#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QFileDialog>
#include <QDockWidget>
#include <QGraphicsDropShadowEffect>
#include <QShortcut>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include "util/IProgressReporter.h"
#include "gl/Global.h"
#include "ctrl/Exporter.h"
#include "gui/MainWindow.h"
#include "gui/ExportDialog.h"
#include "gui/NewProjectDialog.h"
#include "gui/ResourceDialog.h"
#include "gui/ProjectHook.h"
#include "gui/menu/menu_ProgressReporter.h"

namespace
{
class EventSuspender
{
    QWriteLocker mRenderingLocker;
public:
    EventSuspender(gui::MainDisplayWidget& aMainDisplay, gui::TargetWidget& aTarget)
        : mRenderingLocker(&aMainDisplay.renderingLock())
    {
        // stop animation
        aTarget.stop();
    }
};

}

namespace gui
{

MainWindow::MainWindow(ctrl::System& aSystem, GUIResources& aResources, const LocaleParam& aLocaleParam)
    : QMainWindow(nullptr)
    , mSystem(aSystem)
    , mGUIResources(aResources)
    , mViaPoint(this)
    , mKeyCommandMap()
    , mKeyCommandInvoker()
    , mMouseSetting()
    , mMainMenuBar()
    , mMainViewSetting()
    , mMainDisplayStyle()
    , mMainDisplay()
    , mProjectTabBar()
    , mTarget()
    , mProperty()
    , mTool()
    , mResourceDialog()
    , mDriverHolder()
    , mCurrent()
    , mLocaleParam(aLocaleParam)
{
    // setup default opengl format
    {
        QSurfaceFormat format;
#if defined(USE_GL_CORE_PROFILE)
        format.setVersion(gl::Global::kMajorVersion, gl::Global::kMinorVersion);
        format.setProfile(QSurfaceFormat::CoreProfile);
#endif
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);
    }

    // setup UI
    {
        this->setObjectName(QStringLiteral("MainWindow"));
        this->setWindowIcon(QIcon("../src/AnimeEffects.ico"));
        this->setMouseTracking(true);
        this->setFocusPolicy(Qt::NoFocus);
        this->setAcceptDrops(false);
        this->setTabShape(QTabWidget::Rounded);
        this->setDockOptions(QMainWindow::AnimatedDocks);
        this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    }

    // initialize via point
    {
        mViaPoint.setMainViewSetting(mMainViewSetting);

        mMouseSetting.load();
        mViaPoint.setMouseSetting(mMouseSetting);
    }

    // key binding
    {
        mKeyCommandMap.reset(new KeyCommandMap(*this));

        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                           QApplication::organizationName(),
                           QApplication::applicationName());
        settings.beginGroup("keybindings");
        mKeyCommandMap->readFrom(settings);
        settings.endGroup();

        mViaPoint.setKeyCommandMap(mKeyCommandMap.data());

        mKeyCommandInvoker.reset(new KeyCommandInvoker(*mKeyCommandMap));
        mViaPoint.setKeyCommandInvoker(mKeyCommandInvoker.data());
    }

    // create main menu bar
    {
        mMainMenuBar = new MainMenuBar(*this, mViaPoint, mGUIResources, this);
        mViaPoint.setMainMenuBar(mMainMenuBar);
        this->setMenuBar(mMainMenuBar);
    }
    // initialize timer
    {
        timeElapsed.start();
        lastPress = 0;
        lastRelease = 0;
    }

    // create main display
    {
#if defined(Q_OS_WIN)
        const float fontScale = 1.5f;
#else
        const float fontScale = 1.3f;
#endif
        auto font = this->font();
        if (font.pixelSize() > 0) font.setPixelSize((int)(font.pixelSize() * fontScale));
        else font.setPointSizeF(font.pointSizeF() * fontScale);
        mMainDisplayStyle.reset(new MainDisplayStyle(font, mGUIResources));
        mMainDisplay = new MainDisplayWidget(mViaPoint, this);
        this->setCentralWidget(mMainDisplay);

        mProjectTabBar = new ProjectTabBar(mMainDisplay, mGUIResources);
        mMainDisplay->setProjectTabBar(mProjectTabBar);
        mProjectTabBar->onCurrentChanged.connect(this, &MainWindow::onProjectTabChanged);
    }

    // create targeting widget
    {
        QDockWidget* dockWidget = new QDockWidget(this);
        dockWidget->setWindowTitle(tr("Animation Dock"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
        this->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

        mTarget = new TargetWidget(mViaPoint, mGUIResources, dockWidget, QSize(256, 256));
        dockWidget->setWidget(mTarget);
    }

    // create property widget
    {
        QDockWidget* dockWidget = new QDockWidget(this);
        dockWidget->setWindowTitle(tr("Property Dock"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

        mDockPropertyWidget = dockWidget;

#if 0
        mProperty = new PropertyWidget(dockWidget);
        dockWidget->setWidget(mProperty);
#else
        auto splitter = new QSplitter(Qt::Vertical, dockWidget);
        splitter->setObjectName("propertysplitter");
        dockWidget->setWidget(splitter);

        mProperty = new PropertyWidget(mViaPoint, splitter);
        splitter->addWidget(mProperty);

        auto textEdit = new QPlainTextEdit(splitter);
        textEdit->setUndoRedoEnabled(false);
        textEdit->setMaximumBlockCount(32);
        textEdit->setReadOnly(true);
        mViaPoint.setLogView(textEdit);

        splitter->addWidget(textEdit);

        splitter->setCollapsible(0, false);
        splitter->setCollapsible(1, false);
        QList<int> list;
        list.append(9000);
        list.append(1000);
        splitter->setSizes(list);
#endif
    }

    // create tool widget
    {
        QDockWidget* dockWidget = new QDockWidget(this);
        dockWidget->setWindowTitle(tr("Tool Dock"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

        mDockToolWidget = dockWidget;

		mTool = new ToolWidget(mViaPoint, mGUIResources, *mKeyCommandMap, QSize(192, 136), dockWidget);
        dockWidget->setWidget(mTool);
    }

    // create resource dialog
    {
        mResourceDialog = new ResourceDialog(mViaPoint, false, this);
        mViaPoint.setResourceDialog(mResourceDialog);
    }

    // create driver holder
    {
        mDriverHolder.reset(new DriverHolder(mViaPoint));
    }

    // connection
    /// @note Maybe a sequence of connections is meaningful.
    {
        DriverHolder& driver = *mDriverHolder;
        MainDisplayWidget& disp = *mMainDisplay;
        PropertyWidget& prop = *mProperty;
        ToolWidget& tool = *mTool;
        ObjectTreeWidget& objTree = mTarget->objectTreeWidget();
        TimeLineWidget& timeLine = mTarget->timeLineWidget();
        MainMenuBar& menu = *mMainMenuBar;
        ViaPoint& via = mViaPoint;

        driver.onVisualUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);
        tool.onVisualUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);
        prop.onVisualUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);
        objTree.onVisibilityUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);
        menu.onVisualUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);
        via.onVisualUpdated.connect(&disp, &MainDisplayWidget::onVisualUpdated);

        tool.onToolChanged.connect(&disp, &MainDisplayWidget::onToolChanged);
        tool.onFinalizeTool.connect(&disp, &MainDisplayWidget::onFinalizeTool);
        tool.onViewSettingChanged.connect(&disp, &MainDisplayWidget::onViewSettingChanged);

        objTree.onSelectionChanged.connect(&driver, &DriverHolder::onSelectionChanged);
        objTree.onSelectionChanged.connect(mProperty, &PropertyWidget::onSelectionChanged);
        objTree.onSelectionChanged.connect(&timeLine, &TimeLineWidget::onSelectionChanged);

        objTree.onTreeViewUpdated.connect(&timeLine, &TimeLineWidget::onTreeViewUpdated);
        objTree.onScrollUpdated.connect(&timeLine, &TimeLineWidget::onScrollUpdated);

        timeLine.onFrameUpdated.connect(&driver, &DriverHolder::onFrameUpdated);
        timeLine.onFrameUpdated.connect(&prop, &PropertyWidget::onFrameUpdated);
        timeLine.onPlayBackStateChanged.connect(&prop, &PropertyWidget::onPlayBackStateChanged);

        menu.onProjectAttributeUpdated.connect(&disp, &MainDisplayWidget::onProjectAttributeUpdated);
        menu.onProjectAttributeUpdated.connect(&timeLine, &TimeLineWidget::onProjectAttributeUpdated);
        menu.onProjectAttributeUpdated.connect(&driver, &DriverHolder::onProjectAttributeUpdated);
        menu.onTimeFormatChanged.connect(&timeLine, &TimeLineWidget::triggerOnTimeFormatChanged);

        mGUIResources.onThemeChanged.connect(this, &MainWindow::onThemeUpdated);

        mSystem.setAnimator(*mTarget);
    }

    this->setFocusPolicy(Qt::StrongFocus);

#if 0
    auto scUndo = new QShortcut(QKeySequence("Ctrl+Z"), this);
    auto scRedo = new QShortcut(QKeySequence("Ctrl+Shift+Z"), this);
    //scUndo->setContext(Qt::WidgetWithChildrenShortcut);
    //scRedo->setContext(Qt::WidgetWithChildrenShortcut);
    scUndo->setContext(Qt::ApplicationShortcut);
    scRedo->setContext(Qt::ApplicationShortcut);

    this->connect(scUndo, &QShortcut::activated, [=](){ this->onUndoTriggered(); });
    this->connect(scRedo, &QShortcut::activated, [=](){ this->onRedoTriggered(); });
#else
    {
        auto key = mKeyCommandMap->get("Undo");
        if (key) key->invoker = [=](){ this->onUndoTriggered(); };
    }
    {
        auto key = mKeyCommandMap->get("Redo");
        if (key) key->invoker = [=](){ this->onRedoTriggered(); };
    }
    {
        auto key = mKeyCommandMap->get("SaveProject");
        if (key) key->invoker = [=](){ this->onSaveProjectTriggered(); };
    }
    {
        auto key = mKeyCommandMap->get("PlayPause");
        if (key) key->invoker = [=](){ this->onPlayPauseTriggered(); };
    }
    {
        auto key = mKeyCommandMap->get("ToggleDocks");
        if (key) key->invoker = [=](){ this->onDockToggle(); };
    }
    {
        auto key = mKeyCommandMap->get("ToggleRepeat");
        if (key) key->invoker = [=](){ this->onLoopToggle(); };
    }
    {
        auto key = mKeyCommandMap->get("MoveRight");
        if (key) key->invoker = [=](){ this->onDisplacementTriggered(1);};
    }
    {
        auto key = mKeyCommandMap->get("MoveLeft");
        if (key) key->invoker = [=](){ this->onDisplacementTriggered(-1);};
    }
    {
        auto key = mKeyCommandMap->get("MoveToInit");
        if (key) key->invoker = [=](){ this->onMovementTriggered("Init");};
    }
    {
        auto key = mKeyCommandMap->get("MoveToLast");
        if (key) key->invoker = [=](){ this->onMovementTriggered("Last");};
    }
#endif
}

MainWindow::~MainWindow()
{
    closeAllProjects();
}

void MainWindow::showWithSettings()
{
#if defined(QT_NO_DEBUG) || 1
    QSettings settings;
    auto winSize = settings.value("mainwindow/size");
    auto isMax = settings.value("mainwindow/ismaximized");

    if (winSize.isValid())
    {
        this->resize(winSize.toSize());
    }

    if (!isMax.isValid() || isMax.toBool())
    {
        this->showMaximized();
    }
    else
    {
        this->show();
    }
#else
    this->showMaximized();
#endif
}

void MainWindow::saveCurrentSettings(int aResultCode)
{
#if defined(QT_NO_DEBUG) || 1
    if (aResultCode == 0)
    {
        QSettings settings;
        settings.setValue("mainwindow/ismaximized", this->isMaximized());
        if (!this->isMaximized())
        {
            settings.setValue("mainwindow/size", this->size());
        }
    }
#else
    (void)aResultCode;
#endif
}

void MainWindow::testNewProject(const QString& aFilePath)
{
    resetProjectRefs(nullptr);

    menu::ProgressReporter progress(false, this);

    core::Project::Attribute attribute;
    auto result = mSystem.newProject(
                aFilePath, attribute, new ProjectHook(), progress, false);

    if (result)
    {
        resetProjectRefs(result.project);
        mProjectTabBar->pushProject(*result.project);

        mMainDisplay->resetCamera();
    }
}

void MainWindow::closeAllProjects()
{
    mProjectTabBar->removeAllProject();
    resetProjectRefs(nullptr);
    mSystem.closeAllProjects();
}

void MainWindow::resetProjectRefs(core::Project* aProject)
{
    mCurrent = aProject;

    /// @note Maybe a sequence of connections is meaningful.

    if (aProject)
    {
        mDriverHolder->create(*aProject, *mMainDisplayStyle);
    }
    else
    {
        mDriverHolder->destroy();
    }

    mMainMenuBar->setProject(aProject);
    mMainDisplay->setProject(aProject);
    mTarget->setProject(aProject);
    mProperty->setProject(aProject);
    mResourceDialog->setProject(aProject);
    mViaPoint.setProject(aProject);

    mMainDisplay->setDriver(mDriverHolder->driver());
    mTool->setDriver(mDriverHolder->driver());
}

void MainWindow::onProjectTabChanged(core::Project& aProject)
{
    resetProjectRefs(&aProject);
}

void MainWindow::onThemeUpdated(theme::Theme &aTheme)
{
    QFile stylesheet(aTheme.path()+"/stylesheet/standard.ssa");
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString fontOption;
        {
            auto hasFamily = !mLocaleParam.fontFamily.isEmpty();
            auto hasSize = !mLocaleParam.fontSize.isEmpty();
            fontOption = "QWidget {" +
                    (hasFamily ? ("font-family: " + mLocaleParam.fontFamily + ";") : "") +
                    (hasSize ? ("font-size: " + mLocaleParam.fontSize + ";") : "") +
                    " }\n";
        }

        this->setStyleSheet(fontOption + QTextStream(&stylesheet).readAll());

        stylesheet.close();
    }

    stylesheet.setFileName(aTheme.path()+"/stylesheet/propertywidget.ssa");
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mDockPropertyWidget->setStyleSheet(QTextStream(&stylesheet).readAll());
        stylesheet.close();
    }


    stylesheet.setFileName(aTheme.path()+"/stylesheet/toolwidget.ssa");
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mDockToolWidget->setStyleSheet(QTextStream(&stylesheet).readAll());
        stylesheet.close();
    }
}

#if 0
void MainWindow::keyPressEvent(QKeyEvent* aEvent)
{
    //qDebug() << "mainwindow: input key =" << aEvent->key() << "text =" << aEvent->text();
    bool shouldUpdate = false;

    if (mSystem.project())
    {        
        if(aEvent->key() == Qt::Key_Z)
        {
            if (aEvent->modifiers().testFlag(Qt::ControlModifier))
            {
                if (aEvent->modifiers().testFlag(Qt::ShiftModifier))
                {
                    mSystem.project()->commandStack().redo();
                    shouldUpdate = true;
                    qDebug() << "redo";
                }
                else
                {
                    mSystem.project()->commandStack().undo();
                    shouldUpdate = true;
                    qDebug() << "undo";
                }
            }
        }
    }

    if (shouldUpdate)
    {
        mMainDisplay->updateRender();
    }
    /*
    else
    {
        mMainDisplay->throwKeyPress(aEvent);
    }
    */
}
#endif

void MainWindow::keyPressEvent(QKeyEvent* aEvent)
{
    //qDebug() << "input key =" << aEvent->key() << "text =" << aEvent->text();

    if (aEvent->isAutoRepeat()){
        // Delay of 250ms
        if (lastPress + 125 < timeElapsed.elapsed()){
            lastPress = timeElapsed.elapsed();
            mKeyCommandInvoker->onKeyPressed(aEvent);
            QMainWindow::keyPressEvent(aEvent);
        }
        else{
            // qDebug() << "Time elapsed is :" << (lastPress+500)-timeElapsed.elapsed();
            return;
        }
    }
    else{
        mKeyCommandInvoker->onKeyPressed(aEvent);
        QMainWindow::keyPressEvent(aEvent);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* aEvent)
{
    //qDebug() << "release key =" << aEvent->key() << "text =" << aEvent->text();

    if (aEvent->isAutoRepeat()){
        // Delay of 250ms
        if (lastRelease + 125 < timeElapsed.elapsed()){
            lastRelease = timeElapsed.elapsed();
            mKeyCommandInvoker->onKeyReleased(aEvent);
            QMainWindow::keyPressEvent(aEvent);
        }
        else{
            // qDebug() << "time elapsed is :" << (lastPress+500)-timeElapsed.elapsed();
            return;
        }
    }
    else{
        mKeyCommandInvoker->onKeyReleased(aEvent);
        QMainWindow::keyReleaseEvent(aEvent);
    }
}

void MainWindow::closeEvent(QCloseEvent* aEvent)
{
    if (mSystem.hasModifiedProject())
    {
        auto result = confirmProjectClosing(false);

        if (result == QMessageBox::Yes)
        {
            // save all
            for (int i = 0; i < mSystem.projectCount(); ++i)
            {
                auto project = mSystem.project(i);
                XC_PTR_ASSERT(project);
                if (!project->isModified()) continue;

                if (!processProjectSaving(*mSystem.project(i)))
                { // failed or canceled
                    aEvent->ignore();
                    return;
                }
            }
        }
        else if (result == QMessageBox::Cancel)
        {
            aEvent->ignore();
            return;
        }
    }
    aEvent->accept();
}

int MainWindow::confirmProjectClosing(bool aCurrentOnly)
{
    QString singleName;

    if (aCurrentOnly)
    {
        if (mCurrent)
        {
            singleName = mProjectTabBar->getTabName(*mCurrent);
        }
    }
    else
    {
        bool found = false;
        for (int i = 0; i < mSystem.projectCount(); ++i)
        {
            auto project = mSystem.project(i);
            XC_PTR_ASSERT(project);
            if (project->isModified())
            {
                if (found) { singleName.clear(); break; }
                singleName = mProjectTabBar->getTabName(*project);
                found = true;
            }
        }
    }

    QMessageBox msgBox;

    if (!singleName.isEmpty())
    {
        msgBox.setText(singleName + tr(" has been modified. Save changes?"));
    }
    else
    {
        msgBox.setText(tr("Some projects have been modified. Save changes?"));
    }

    msgBox.addButton(tr("Save Changes"), QMessageBox::YesRole);
    msgBox.addButton(tr("Discard Changes"), QMessageBox::NoRole);
    auto cancel = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancel);
    msgBox.exec();
    auto clicked = msgBox.clickedButton();
    if (clicked)
    {
        auto role = msgBox.buttonRole(clicked);
        if (role == QMessageBox::YesRole) return QMessageBox::Yes;
        else if (role == QMessageBox::NoRole) return QMessageBox::No;
        else if (role == QMessageBox::RejectRole) return QMessageBox::Cancel;
    }
    return QMessageBox::Cancel;
}

void MainWindow::onUndoTriggered()
{
    if (mCurrent)
    {
        bool undone = false;
        auto ret = mCurrent->commandStack().undo(&undone);
        if (undone)
        {
            mViaPoint.pushUndoneLog(tr("Undone : ") + ret);
            mMainDisplay->updateRender();
        }
    }
}

void MainWindow::onDisplacementTriggered(int frameDisplacement)
{
    mTarget->timeLineWidget().setFrame(mTarget->currentFrame().added(frameDisplacement));
}

void MainWindow::onPlayPauseTriggered()
{
    mTarget->playBackWidget().PlayPause(); // If it's not playing then it will begin playback
}

void MainWindow::onMovementTriggered(QString frameMovement)
{
    if (frameMovement == "Init"){
    mTarget->timeLineWidget().setFrame(core::Frame(mCurrent->attribute().maxFrame()));
    }
    else{
    mTarget->timeLineWidget().setFrame(core::Frame(0));
    }
}

void MainWindow::onDockToggle() // For some reason it does not work without this travesty
{
    if (mDockPropertyWidget->isHidden()){mDockPropertyWidget->show();}
    else if (!mDockPropertyWidget->isHidden()){mDockPropertyWidget->hide();}

    if (mDockToolWidget->isHidden()) {mDockToolWidget->show();}
    else if (!mDockToolWidget->isHidden()) {mDockToolWidget->hide();}

    if (mTarget->isHidden()){mTarget->show();}
    else if (!mTarget->isHidden()){mTarget->hide();}
}

void MainWindow::onLoopToggle()
{
    if (mTarget->playBackWidget().isLoopChecked()){
        mTarget->timeLineWidget().setPlayBackLoop(false);
        mTarget->playBackWidget().checkLoop(false);
    }
    else{
        mTarget->timeLineWidget().setPlayBackLoop(true);
        mTarget->playBackWidget().checkLoop(true);
    }
}

void MainWindow::onRedoTriggered()
{
    if (mCurrent)
    {
        bool redone = false;
        auto ret = mCurrent->commandStack().redo(&redone);
        if (redone)
        {
            mViaPoint.pushRedoneLog(tr("Redone : ") + ret);
            mMainDisplay->updateRender();
        }
    }
}

void MainWindow::onNewProjectTriggered()
{
    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    // input attribute
    core::Project::Attribute attribute;
    QString fileName;
    bool specifiesCanvasSize = false;
    {
        QScopedPointer<NewProjectDialog> dialog(new NewProjectDialog(this));

        dialog->exec();
        if (dialog->result() != QDialog::Accepted)
        {
            return;
        }
        attribute = dialog->attribute();
        fileName = dialog->fileName();
        specifiesCanvasSize = dialog->specifiesCanvasSize();
    }

    // clear old project
    resetProjectRefs(nullptr);

    // try loading
    ctrl::System::LoadResult result;
    {
        menu::ProgressReporter progress(false, this);

        result = mSystem.newProject(
                    fileName, attribute, new ProjectHook(),
                    progress, specifiesCanvasSize);
    }

    if (result.project)
    {
        resetProjectRefs(result.project);
        mProjectTabBar->pushProject(*result.project);

        mMainDisplay->resetCamera();
    }
    else
    {
        QMessageBox::warning(nullptr, tr("Loading Error"), result.messages());

        if (mProjectTabBar->currentProject())
        {
            resetProjectRefs(mProjectTabBar->currentProject());
        }
    }
}

QFileSystemWatcher* globalWatcher = new QFileSystemWatcher();

QFileSystemWatcher* MainWindow::getWatcher(){
    return globalWatcher;
}

void MainWindow::showInfoPopup(const QString& aTitle, const QString& aDetailText, const QString& aIcon, const QString& aDetailed)
{
    QMessageBox box;
    if(aIcon == "Info"){
        box.setIcon(QMessageBox::Information);
    }
    else if (aIcon == "Warn"){
        box.setIcon(QMessageBox::Warning);
    }
    else if (aIcon == "Critical"){
        box.setIcon(QMessageBox::Critical);
    }
    box.setStandardButtons(QMessageBox::Ok);
    box.setDefaultButton(QMessageBox::Ok);
    box.setWindowTitle(aTitle);
    box.setText(aDetailText);
    if (aDetailed != "nullptr"){
        box.setDetailedText(aDetailed);
    }
    box.exec();
}

void MainWindow::onOpenProjectTriggered()
{
    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open File"), "", "ProjectFile (*.anie)");
    if (fileName.isEmpty()) return;

    // clear old project
    resetProjectRefs(nullptr);

    // try loading
    ctrl::System::LoadResult result;
    {
        menu::ProgressReporter progress(false, this);
        result = mSystem.openProject(fileName, new ProjectHook(), progress);
    }

    if (result)
        {
        resetProjectRefs(result.project);
        mProjectTabBar->pushProject(*result.project);

        mMainDisplay->resetCamera();
        }
    else
    {
        QMessageBox::warning(nullptr, tr("Loading Error"), result.messages());

        if (mProjectTabBar->currentProject())
        {
            resetProjectRefs(mProjectTabBar->currentProject());
        }
    }
}

void MainWindow::onOpenRecentTriggered(QString aFileName)
{
    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    QString fileName = aFileName;

    // clear old project
    resetProjectRefs(nullptr);

    // try loading
    ctrl::System::LoadResult result;
    {
        menu::ProgressReporter progress(false, this);
        result = mSystem.openProject(fileName, new ProjectHook(), progress);
    }

    if (result)
    {
        resetProjectRefs(result.project);
        mProjectTabBar->pushProject(*result.project);

        mMainDisplay->resetCamera();
    }
    else
    {
        QMessageBox::warning(nullptr, tr("Loading Error"), result.messages());

        if (mProjectTabBar->currentProject())
        {
            resetProjectRefs(mProjectTabBar->currentProject());
        }
    }
}

bool MainWindow::processProjectSaving(core::Project& aProject, bool aRename)
{
    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    if (aProject.isNameless() || aRename)
    {
        QString fileName = QFileDialog::getSaveFileName(
                    this, tr("Save File"), "", "ProjectFile (*.anie)");

        // check cancel
        if (fileName.isEmpty())
        {
            return false;
        }

        if (QFileInfo(fileName).suffix().isEmpty())
        {
            fileName += ".anie";
        }
        aProject.setFileName(fileName);
    }

    // save
    auto result = mSystem.saveProject(aProject);
    if (!result)
    {
        QMessageBox::warning(nullptr, "Saving Error", result.message);
        return false; // failed
    }

    mProjectTabBar->updateTabNames();
    return true;
}

void MainWindow::onSaveProjectTriggered()
{
    if (mCurrent)
    {
        processProjectSaving(*mCurrent);
    }
}

void MainWindow::onSaveProjectAsTriggered()
{
    if (mCurrent)
    {
        processProjectSaving(*mCurrent, true);
    }
}

void MainWindow::onCloseProjectTriggered()
{
    if (mCurrent)
    {

        // Sneaky potential crash
        QFileSystemWatcher* watcher = MainWindow::getWatcher();
        for (unsigned int x = 0; x < mCurrent->resourceHolder().imageTrees().size(); x += 1){
            if (watcher->files().contains(mCurrent->resourceHolder().findAbsoluteFilePath(*mCurrent->resourceHolder().imageTree(x).topNode))){
                watcher->removePath(mCurrent->resourceHolder().findAbsoluteFilePath(*mCurrent->resourceHolder().imageTree(x).topNode));
            }
        }

        if (mCurrent->isModified())
        {
            auto result = confirmProjectClosing(true);

            if (result == QMessageBox::Cancel)
            {
                return;
            }
            else if (result == QMessageBox::Yes && !processProjectSaving(*mCurrent))
            {
                return;
            }
        }

        auto closeProject = mCurrent;
        mProjectTabBar->removeProject(*closeProject);
        resetProjectRefs(nullptr); ///@note update mCurrent
        mSystem.closeProject(*closeProject);

        if (mProjectTabBar->currentProject())
        {
            resetProjectRefs(mProjectTabBar->currentProject());
        }
    }
}

void MainWindow::onExportImageSeqTriggered(const QString& aSuffix)
{
    if (!mCurrent) return;

    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    // export directory
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Export Folder"));

    // make sure existing
    if (dirName.isEmpty()) return;
    if (!QFileInfo(dirName).exists()) return;

    // export param
    ctrl::Exporter::CommonParam cparam;
    ctrl::Exporter::ImageParam iparam;
    {
        QScopedPointer<ImageExportDialog> dialog(
                    new ImageExportDialog(*mCurrent, dirName, aSuffix, this));

        dialog->exec();
        if (dialog->result() != QDialog::Accepted) return;

        cparam = dialog->commonParam();
        iparam = dialog->imageParam();
    }

    // gui for confirm overwrite
    auto overwriteConfirmer = [=](const QString&) -> bool
    {
        QMessageBox msgBox;
        msgBox.setText(tr("File already exists."));
        msgBox.setInformativeText(tr("Do you want to overwrite the existing file?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        return (ret == QMessageBox::Ok);
    };

    menu::ProgressReporter progress(true, this);
    ctrl::Exporter exporter(*mCurrent);
    exporter.setOverwriteConfirmer(overwriteConfirmer);
    exporter.setProgressReporter(progress);

    // execute
    if (!exporter.execute(cparam, iparam))
    {
        progress.cancel();

        if (!exporter.isCanceled())
        {
            QMessageBox::warning(nullptr, tr("Export Error"), exporter.log());
        }
        return;
    }
}
void MainWindow::onExportVideoTriggered(const ctrl::VideoFormat& aFormat)
{
    if (!mCurrent) return;

    // stop animation and main display rendering
    EventSuspender suspender(*mMainDisplay, *mTarget);

    const QString suffix = aFormat.name;
    const QString targetVideos = "Videos (*." + suffix + ")";
    const bool isGif = (suffix == "gif");

    // get export file name
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Export File"),
                QString(), // dir
                targetVideos);
    const QFileInfo fileInfo(fileName);

    // make sure existing
    if (fileName.isEmpty()) return;
    if (!fileInfo.dir().exists()) return;

    if (fileInfo.suffix().isEmpty())
    {
        fileName += "." + suffix; // makesure suffix
    }
    else if (fileInfo.suffix() != suffix)
    {
        QMessageBox::warning(nullptr, tr("Operation Error"),
                             tr("Invalid extension specified."));
        return;
    }

    // export param
    ctrl::Exporter::CommonParam cparam;
    ctrl::Exporter::VideoParam vparam;
    ctrl::Exporter::GifParam gparam;
    if (isGif)
    {
        QScopedPointer<GifExportDialog> dialog(
                    new GifExportDialog(*mCurrent, fileName, this));
        dialog->exec();
        if (dialog->result() != QDialog::Accepted) return;

        cparam = dialog->commonParam();
        gparam = dialog->gifParam();
    }
    else
    {
        QScopedPointer<VideoExportDialog> dialog(
                    new VideoExportDialog(
                        *mCurrent, fileName, aFormat, this));
        dialog->exec();
        if (dialog->result() != QDialog::Accepted) return;

        cparam = dialog->commonParam();
        vparam = dialog->videoParam();
    }
    //vparam.codec = codec;

    menu::LoggableProgressReporter progress(true, this);
    ctrl::Exporter exporter(*mCurrent);
    exporter.setOverwriteConfirmer([=](const QString&)->bool { return true; });
    exporter.setProgressReporter(progress);
    exporter.setUILogger(progress);

    // execute
    auto result = isGif ?
                exporter.execute(cparam, gparam) :
                exporter.execute(cparam, vparam);

    if (!result)
    {
        progress.cancel();

        if (result.code == ctrl::Exporter::ResultCode_Canceled)
        {
        }
        else if (result.code == ctrl::Exporter::ResultCode_FFMpegFailedToStart)
        {
            QMessageBox message;
            message.setIcon(QMessageBox::Warning);
            message.setText(tr("FFmpeg was not found."));
            auto infoText =
                    tr("Video export requires FFmpeg.") + "\n" +
                    tr("Install FFmpeg on the system, or place a FFmpeg executable "
                       "under \"/tools\" in the folder where you installed AnimeEffects.");
            message.setInformativeText(infoText);
            message.setStandardButtons(QMessageBox::Ok);
            message.setDefaultButton(QMessageBox::Ok);
            message.exec();
        }
        else
        {
            QMessageBox::warning(nullptr, tr("Export Error"), exporter.log());
        }
    }
}

} // namespace gui
