#include <QApplication>
#include <QSettings>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <qstandardpaths.h>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "gui/GeneralSettingDialog.h"
#include "util/NetworkUtil.h"

namespace {

static const int kLanguageTypeCount = 4;
static const int kTimeFormatTypeCount = 6;
static const int kEasingTypeCount = 12;
static const int kRangeTypeCount = 3;

int languageToIndex(const QString& aLanguage) {
    if (aLanguage == "Auto")
        return 0;
    else if (aLanguage == "English")
        return 1;
    else if (aLanguage == "Japanese")
        return 2;
    else if (aLanguage == "Chinese")
        return 3;
    else
        return 0;
}

int easingToIndex(const QString& aEasing) {
    if (aEasing == "None")
        return 0;
    else if (aEasing == "Linear")
        return 1;
    else if (aEasing == "Sine")
        return 2;
    else if (aEasing == "Quad")
        return 3;
    else if (aEasing == "Cubic")
        return 4;
    else if (aEasing == "Quart")
        return 5;
    else if (aEasing == "Quint")
        return 6;
    else if (aEasing == "Expo")
        return 7;
    else if (aEasing == "Circ")
        return 8;
    else if (aEasing == "Back")
        return 9;
    else if (aEasing == "Elastic")
        return 10;
    else if (aEasing == "Bounce")
        return 11;
    else
        return 1; // Default easing is Linear
}

int rangeToIndex(const QString& aRange) {
    if (aRange == "In")
        return 0;
    else if (aRange == "Out")
        return 1;
    else if (aRange == "All")
        return 2;
    else
        return 2; // Default range is InOut, it is referenced as "All" by Hidefuku
}

QString indexToEasing(int aIndex, bool translated = true) {
    if (translated) {
        switch (aIndex) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "None");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "Linear");
        case 2:
            return QCoreApplication::translate("GeneralSettingsDialog", "Sine");
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quad");
        case 4:
            return QCoreApplication::translate("GeneralSettingsDialog", "Cubic");
        case 5:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quart");
        case 6:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quint");
        case 7:
            return QCoreApplication::translate("GeneralSettingsDialog", "Expo");
        case 8:
            return QCoreApplication::translate("GeneralSettingsDialog", "Circ");
        case 9:
            return QCoreApplication::translate("GeneralSettingsDialog", "Back");
        case 10:
            return QCoreApplication::translate("GeneralSettingsDialog", "Elastic");
        case 11:
            return QCoreApplication::translate("GeneralSettingsDialog", "Bounce");
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "Linear");
        }
    } else {
        switch (aIndex) {
        case 0:
            return QString("None");
        case 1:
            return QString("Linear");
        case 2:
            return QString("Sine");
        case 3:
            return QString("Quad");
        case 4:
            return QString("Cubic");
        case 5:
            return QString("Quart");
        case 6:
            return QString("Quint");
        case 7:
            return QString("Expo");
        case 8:
            return QString("Circ");
        case 9:
            return QString("Back");
        case 10:
            return QString("Elastic");
        case 11:
            return QString("Bounce");
        default:
            return QString("Linear");
        }
    }
}

QString indexToRange(int aRange, bool translated = true) {
    if (translated) {
        switch (aRange) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "In");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "Out");
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "All");
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "All");
        }
    } else {
        switch (aRange) {
        case 0:
            return QString("In");
        case 1:
            return QString("Out");
        case 3:
            return QString("All");
        default:
            return QString("All");
        }
    }
}

QString indexToLanguage(int aIndex, bool translated = true) {
    if (translated) {
        switch (aIndex) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "Auto");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "English");
            ;
        case 2:
            return QCoreApplication::translate("GeneralSettingsDialog", "Japanese");
            ;
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "Chinese");
            ;
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "Auto");
            ;
        }
    } else {
        switch (aIndex) {
        case 0:
            return QString("Auto");
        case 1:
            return QString("Enslish");
        case 2:
            return QString("Japanese");
        case 3:
            return QString("Chinese");
        default:
            return QString("Auto");
        }
    }
}
} // namespace

QString indexToTimeFormat(int aIndex) {
    switch (aIndex) {
    case core::TimeFormatType::TimeFormatType_Frames_From0:
        return QCoreApplication::translate("GeneralSettingsDialog", "Frame number (from 0)");
    case core::TimeFormatType::TimeFormatType_Frames_From1:
        return QCoreApplication::translate("GeneralSettingsDialog", "Frame number (from 1)");
    case core::TimeFormatType::TimeFormatType_Relative_FPS:
        return QCoreApplication::translate("GeneralSettingsDialog", "Relative to FPS (1.0 = 60.0)");
    case core::TimeFormatType::TimeFormatType_Seconds_Frames:
        return QCoreApplication::translate("GeneralSettingsDialog", "Seconds : Frame");
    case core::TimeFormatType::TimeFormatType_Timecode_SMPTE:
        return QCoreApplication::translate("GeneralSettingsDialog", "Timecode (SMPTE) (HH:MM:SS:FF)");
    case core::TimeFormatType::TimeFormatType_Timecode_HHMMSSmmm:
        return QCoreApplication::translate("GeneralSettingsDialog", "Timecode (HH:MM:SS:mmm)");
    default:
        return "";
    }
}


namespace gui {

GeneralSettingDialog::GeneralSettingDialog(GUIResources& aGUIResources, QWidget* aParent):
    EasyDialog(tr("General Settings"), aParent),
    mTabs(new QTabWidget(this)),
    mInitialLanguageIndex(),
    mLanguageBox(),
    mInitialTimeFormatIndex(),
    mTimeFormatBox(),
    mInitialThemeKey("default"),
    mThemeBox(),
    mGUIResources(aGUIResources) {
    // read current settings
    {
        QSettings settings;
        auto language = settings.value("generalsettings/language");

        if (language.isValid()) {
            mInitialLanguageIndex = languageToIndex(language.toString());
        }

        auto easing = settings.value("generalsettings/easing");
        mInitialEasingIndex = easing.isValid() ? easingToIndex(easing.toString()) : easingToIndex("Linear");

        auto range = settings.value("generalsettings/range");
        mInitialRangeIndex = range.isValid() ? rangeToIndex(range.toString()) : rangeToIndex("All");

        auto timeScale = settings.value("generalsettings/ui/timeformat");
        if (timeScale.isValid()) {
            mInitialTimeFormatIndex = timeScale.toInt();
        }

        auto theme = settings.value("generalsettings/ui/theme");
        if (theme.isValid()) {
            mInitialThemeKey = theme.toString();
        }

        auto isAutoSave = settings.value("generalsettings/projects/autosaveEnabled");
        bAutoSave = isAutoSave.isValid() && isAutoSave.toBool();

        auto isAutoSaveDelay = settings.value("generalsettings/projects/autosaveDelay");
        mAutoSaveDelay = isAutoSaveDelay.isValid() ? isAutoSaveDelay.toInt() : 5;

        auto isAutoCbCopy = settings.value("generalsettings/keys/autocb");
        bAutoCbCopy = !isAutoCbCopy.isValid() || isAutoCbCopy.toBool();

        auto isAutoFFmpegCheck = settings.value("ffmpeg_check");
        mAutoFFmpegCheck = isAutoFFmpegCheck.isValid()? true : isAutoFFmpegCheck.toBool();

        auto isKeyDelay = settings.value("generalsettings/keybindings/keyDelay");
        mKeyDelay = isKeyDelay.isValid() ? isKeyDelay.toInt() : 125;

        auto isAutoShowMesh = settings.value("generalsettings/tools/autoshowmesh");
        bAutoShowMesh = isAutoShowMesh.isValid() && isAutoShowMesh.toBool();
    }

    auto form = new QFormLayout();

    // create inner widgets
    {
        mLanguageBox = new QComboBox();
        for (int i = 0; i < kLanguageTypeCount; ++i) {
            mLanguageBox->addItem(indexToLanguage(i));
        }
        mLanguageBox->setCurrentIndex(mInitialLanguageIndex);
        form->addRow(tr("Language (needs restarting) :"), mLanguageBox);

        mEasingBox = new QComboBox();
        for (int i = 0; i < kEasingTypeCount; ++i) {
            mEasingBox->addItem(indexToEasing(i));
        }
        mEasingBox->setCurrentIndex(mInitialEasingIndex);
        form->addRow(tr("Default keyframe easing :"), mEasingBox);

        mRangeBox = new QComboBox();
        for (int i = 0; i < kRangeTypeCount; ++i) {
            mRangeBox->addItem(indexToRange(i));
        }
        mRangeBox->setCurrentIndex(mInitialRangeIndex);
        form->addRow(tr("Default keyframe range :"), mRangeBox);

        mTimeFormatBox = new QComboBox();
        for (int i = 0; i < kTimeFormatTypeCount; ++i) {
            mTimeFormatBox->addItem(indexToTimeFormat(i));
        }
        mTimeFormatBox->setCurrentIndex(mInitialTimeFormatIndex);
        form->addRow(tr("Timeline format :"), mTimeFormatBox);


        mThemeBox = new QComboBox();
        QStringList themeList = mGUIResources.themeList();
        for (int i = 0; i < themeList.size(); ++i) {
            mThemeBox->addItem(themeList[i], themeList[i]);
        }
        mThemeBox->setCurrentIndex(mThemeBox->findData(mInitialThemeKey));
        form->addRow(tr("Theme :"), mThemeBox);
    }

    auto projectSaving = new QFormLayout();
    {
        mAutoSave = new QCheckBox();
        mAutoSave->setChecked(bAutoSave);
        projectSaving->addRow(tr("Automatically save your project : "), mAutoSave);

        mAutoSaveDelayBox = new QSpinBox();
        mAutoSaveDelayBox->setValue(mAutoSaveDelay);
        projectSaving->addRow(tr("Time in minutes between autosaves : "), mAutoSaveDelayBox);

        mAutoShowMesh = new QCheckBox();
        mAutoShowMesh->setChecked(bAutoShowMesh);
        connect(mAutoShowMesh, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.setValue("generalsettings/tools/autoshowmesh", mAutoShowMesh->isChecked());
        });
        projectSaving->addRow(tr("Automatically show mesh when selecting FFD : "), mAutoShowMesh);

        mAutoFFmpegBox = new QCheckBox();
        mAutoFFmpegBox->setChecked(mAutoFFmpegCheck);
        projectSaving->addRow(tr("Check for FFmpeg on export : "), mAutoFFmpegBox);

        mResetButton = new QPushButton(tr("Reset recent files list"));
        mResetButton->setToolTip(tr("Deletes all project entries from your recents"));
        connect(mResetButton, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.remove("projectloader/recents");
            MainWindow::showInfoPopup(tr("Success"), tr("All entries have been successfully removed"), "Info");
        });
        projectSaving->addRow(mResetButton);
    }

    auto keysettings = new QFormLayout();
    {
        mAutoCbCopy = new QCheckBox();
        mAutoCbCopy->setChecked(bAutoCbCopy);
        keysettings->addRow(tr("Automatically copy keys to the clipboard"), mAutoCbCopy);

        /*
        mHSVBlendColor = new QCheckBox();
        mHSVBlendColor->setChecked(bHSVBlendColor);
        keysettings->addRow(tr("HSV | Blend color : "), mHSVBlendColor);

        mHSVFolder = new QCheckBox();
        mHSVFolder->setChecked(bHSVFolder);
        keysettings->addRow(tr("HSV | Enable folder support (beta)"), mHSVFolder);

        mHSVBehaviour = new QComboBox();
        mHSVBehaviour->addItem(tr("Render indefinitely"));
        mHSVBehaviour->addItem(tr("Only render between keys"));
        mHSVBehaviour->setCurrentIndex(mInitialHSVBehaviour);
        keysettings->addRow(tr("HSV | Key rendering (Needs reboot) : "), mHSVBehaviour);
        */
    }

    auto keybindingSettings = new QFormLayout();
    {
        mKeyDelayBox = new QSpinBox();
        mKeyDelayBox->setRange(0, 10000);
        mKeyDelayBox->setValue(mKeyDelay);
        keybindingSettings->addRow(tr("Global keybind delay (ms) : "), mKeyDelayBox);

        mResetKeybindsButton = new QPushButton(tr("Reset keybinds"));
        mResetKeybindsButton->setToolTip(tr("Reset all keybinds, a restart is required."));
        connect(mResetKeybindsButton, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.setValue("keybindReset",
                !settings.value("keybindReset").isValid() || !settings.value("keybindReset").toBool()
            );
            MainWindow::showInfoPopup(
                tr("Keybinds reset status"),
                settings.value("keybindReset").toBool() ? tr("The keybinds will be reset when you restart.")
                                                        : tr("The keybinds will not be reset when you restart."),
                "Info"
            );
        });
        keybindingSettings->addRow(mResetKeybindsButton);
    }

    auto ffmpegSettings = new QFormLayout();
    {

        ffmpegTroubleshoot = new QPushButton(tr("Troubleshoot FFmpeg"));
        connect(ffmpegTroubleshoot, &QPushButton::clicked, [=]() {
            util::NetworkUtil networking;
            QFileInfo ffmpeg_file;
            QString ffmpeg;
            QMessageBox ffmpegNotif;

            if (util::NetworkUtil::os() == "win") {
                ffmpeg_file = QFileInfo("./tools/ffmpeg.exe");
            } else {
                ffmpeg_file = QFileInfo("./tools/ffmpeg");
            }
            if (!ffmpeg_file.exists() || !ffmpeg_file.isExecutable()) {
                ffmpeg = "ffmpeg";
            } else {
                ffmpeg = ffmpeg_file.absoluteFilePath();
            }

            // Exists?
            bool fExists = util::NetworkUtil::libExists(ffmpeg, "-version");

            if (!fExists) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg error"));
                ffmpegNotif.setText(tr("FFmpeg is either missing, corrupted or otherwise doesn't work."));
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }
            // Sample gif test
            QProcess gif;
            const QStringList testInput = {"-f", "lavfi", "-i", "testsrc=duration=1:size=24x20:rate=2"};
            gif.start(ffmpeg, testInput + QStringList{"gif.gif"}, QProcess::ReadWrite);
            gif.waitForFinished();
            bool exportSuccess = gif.exitStatus() == 0 && QFileInfo::exists("gif.gif");
            qDebug() << "Gif exists: " << QFileInfo::exists("gif.gif")
                     << "| Gif remove: " << QFile("gif.gif").remove();
            gif.deleteLater();

            if (!exportSuccess) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg doesn't export"));
                ffmpegNotif.setText(tr("FFmpeg was unable to export, please check if it's a valid FFmpeg executable."));
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }

            // Palettegen test
            QProcess palettegen;
            palettegen.start(ffmpeg, testInput + QStringList{"-vf", "palettegen", "palette.png"}, QProcess::ReadWrite);
            palettegen.waitForFinished();
            bool pGenSuccess = palettegen.exitStatus() == 0 && QFileInfo::exists("palette.png");
            if (!pGenSuccess) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg doesn't generate palettes"));
                ffmpegNotif.setText(
                    tr("FFmpeg was unable to generate palettes, please check if it's a valid FFmpeg executable.")
                );
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }
            qDebug() << "Palette exists: " << QFileInfo::exists("palette.png")
                     << "| Palette remove: " << QFile("palette.png").remove();
            palettegen.deleteLater();


            ffmpegNotif.setWindowTitle(tr("FFmpeg test success"));
            ffmpegNotif.setText(tr("All tests have passed, FFmpeg is working correctly."));
            ffmpegNotif.setDetailedText(
                tr("Tests:\nCheck FFmpeg version ... 🗸\nCheck FFmpeg exporting ... 🗸\n"
                   "Check FFmpeg palette generation ... 🗸")
            );
            ffmpegNotif.addButton(QMessageBox::Ok);
            ffmpegNotif.exec();
            return;
        });

        selectFromExe = new QPushButton(tr("Select from executable and automatically setup"));
        selectFromExe->setToolTip(
            tr("This will remove previous instances of FFmpeg from your tools directory"
               " and replace them with your custom executable, please make sure this is a valid FFmpeg executable.")
        );
        connect(selectFromExe, &QPushButton::clicked, [=]() {
            util::NetworkUtil net;
            QDir dir = QDir("./tools");
            if (!dir.exists()) {
                dir.mkpath(dir.absolutePath());
            }
            QString file = util::NetworkUtil::os() == "win" ? "/ffmpeg.exe" : "/ffmpeg";

            if (QFileInfo::exists(dir.absolutePath() + file)) {
                QFile(dir.absolutePath() + file).moveToTrash();
            }

            QFileInfo ffmpeg = QFileInfo(QFileDialog::getOpenFileName(nullptr, "Select FFmpeg executable"));
            qDebug() << "Selected exe: " << ffmpeg.fileName() << " | " << ffmpeg.absoluteFilePath();
            QFile(ffmpeg.absoluteFilePath()).copy(dir.absolutePath() + file);

            QMessageBox success;
            success.setText(tr("Operation successful"));
            success.setWindowTitle(tr("Success"));
            success.addButton(QMessageBox::Ok);
            success.exec();
        });


        autoSetup = new QPushButton(tr("Download and automatically setup"));
        connect(autoSetup, &QPushButton::clicked, [=]() {
            util::NetworkUtil networking;

            QDir dir = QDir("./tools");
            if (!dir.exists()) {
                dir.mkpath(dir.absolutePath());
            }
            QString file = util::NetworkUtil::os() == "win" ? "/ffmpeg.exe" : "/ffmpeg";
            if (QFileInfo::exists(dir.absolutePath() + file)) {
                QFile(dir.absolutePath() + file).moveToTrash();
            }
            QString os = util::NetworkUtil::os();
            QString arch = util::NetworkUtil::arch();
            QString gitFile;
            // ID checking is probably overkill, but security is important!
            int id;
            if (os == "win") {
                if (arch == "x86") {
                    gitFile = "ffmpeg-win32.exe";
                    id = 97679467;
                } else {
                    gitFile = "ffmpeg-win64.exe";
                    id = 97679053;
                }
            } else if (os == "linux") {
                if (arch == "x86") {
                    gitFile = "ffmpeg-linux32";
                    id = 97679324;
                } else {
                    gitFile = "ffmpeg-linux64";
                    id = 97679172;
                }
            } else {
                gitFile = "ffmpeg-macos";
                id = 97679141;
            }

            QFileInfo ffmpeg = networking.downloadGithubFile(
                "https://api.github.com/repos/AnimeEffectsDevs/ffmpeg-bin/releases/latest", gitFile, id, this
            );
            qDebug() << "Download name : " << ffmpeg.fileName() << "\n"
                     << "Download is executable : " << ffmpeg.isExecutable();
            if (ffmpeg.isExecutable()) {
                QFile(ffmpeg.absoluteFilePath()).rename(dir.absolutePath() + file);
                qDebug() << "FFmpeg moved : " << QFile(dir.absolutePath() + file).exists();
                if (QFile(dir.absolutePath() + file).exists()) {
                    QMessageBox success;
                    success.setWindowTitle(tr("Success"));
                    success.setText(tr("FFmpeg was successfully setup."));
                    success.exec();
                    return;
                }
            }
            QMessageBox error;
            error.setWindowTitle(tr("Error"));
            error.setText(tr("An error has occurred, please send the below info to the developers."));
            error.setDetailedText(
                "Filename : " + ffmpeg.fileName() + "\nIs Executable : " + (ffmpeg.isExecutable()? "True" : "False") +
                "\nDownload params : " + "Hardcoded API | gitFile: " + gitFile + " | ID: " + QString::number(id) +
                "\nDownload path : " + dir.absolutePath()
            );
            error.exec();
        });

        ffmpegSettings->addRow(ffmpegTroubleshoot);
        ffmpegSettings->addRow(selectFromExe);
        ffmpegSettings->addRow(autoSetup);
    };

    createTab(tr("General"), form);
    createTab(tr("Project and Tools"), projectSaving);
    createTab(tr("FFmpeg"), ffmpegSettings);
    createTab(tr("Animation keys"), keysettings);
    createTab(tr("Keybindings"), keybindingSettings);


    this->setMainWidget(mTabs, false);

    this->setOkCancel([=](int aResult) -> bool {
        if (aResult == 0) {
            this->saveSettings();
        }
        return true;
    });
}

QFormLayout* GeneralSettingDialog::createTab(const QString& aTitle, QFormLayout* aForm) {
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    auto frame = new QFrame();
    scroll->setWidget(frame);

    auto form = new QFormLayout();
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form->setLabelAlignment(Qt::AlignRight);
    frame->setLayout(aForm);

    mTabs->addTab(scroll, aTitle);

    return form;
}

bool GeneralSettingDialog::languageHasChanged() { return (mInitialLanguageIndex != mLanguageBox->currentIndex()); }

bool GeneralSettingDialog::easingHasChanged() { return (mInitialEasingIndex != mEasingBox->currentIndex()); }

bool GeneralSettingDialog::rangeHasChanged() { return (mInitialRangeIndex != mRangeBox->currentIndex()); }

bool GeneralSettingDialog::timeFormatHasChanged() {
    return (mInitialTimeFormatIndex != mTimeFormatBox->currentIndex());
}

bool GeneralSettingDialog::themeHasChanged() { return (mInitialThemeKey != mThemeBox->currentData()); }

bool GeneralSettingDialog::autoSaveHasChanged() { return (bAutoSave != mAutoSave->isChecked()); }

bool GeneralSettingDialog::autoSaveDelayHasChanged() { return (mAutoSaveDelay != mAutoSaveDelayBox->value()); }

bool GeneralSettingDialog::autoFFmpegHasChanged() { return (mAutoFFmpegCheck != mAutoFFmpegBox->isChecked()); }

bool GeneralSettingDialog::cbCopyHasChanged() { return (bAutoCbCopy != mAutoCbCopy->isChecked()); }

QString GeneralSettingDialog::theme() { return mThemeBox->currentData().toString(); }

bool GeneralSettingDialog::keyDelayHasChanged() { return (mKeyDelay != mKeyDelayBox->value()); }

void GeneralSettingDialog::saveSettings() {
    QSettings settings;
    if (languageHasChanged())
        settings.setValue("generalsettings/language", indexToLanguage(mLanguageBox->currentIndex(), false));

    if (easingHasChanged()) {
        settings.setValue("generalsettings/easing", indexToEasing(mEasingBox->currentIndex(), false));
    }
    if (rangeHasChanged()) {
        settings.setValue("generalsettings/range", indexToRange(mRangeBox->currentIndex(), false));
    }
    if (timeFormatHasChanged()) {
        settings.setValue("generalsettings/ui/timeformat", mTimeFormatBox->currentIndex());
    }
    if (themeHasChanged()) {
        settings.setValue("generalsettings/ui/theme", mThemeBox->currentData());
    }
    if (autoSaveHasChanged()) {
        settings.setValue("generalsettings/projects/autosaveEnabled", mAutoSave->isChecked());
    }
    if (autoSaveDelayHasChanged()) {
        settings.setValue("generalsettings/projects/autosaveDelay", mAutoSaveDelayBox->value());
    }
    if (autoFFmpegHasChanged()){
        settings.setValue("ffmpeg_check", mAutoFFmpegBox->isChecked());
    }
    if (cbCopyHasChanged()) {
        settings.setValue("generalsettings/keys/autocb", mAutoCbCopy->isChecked());
    }
    /*
    if (HSVBehaviourHasChanged()){
        settings.setValue("generalsettings/keys/hsvBehaviour", mHSVBehaviour->currentIndex());
    }
    if (HSVSetColorHasChanged()){
        settings.setValue("generalsettings/keys/hsvSetColor", mHSVBlendColor->isChecked());
    }
    if (HSVFolderHasChanged()){
        settings.setValue("generalsettings/keys/hsvFolder", mHSVFolder->isChecked());
    }
    */
    if (keyDelayHasChanged()) {
        settings.setValue("generalsettings/keybindings/keyDelay", mKeyDelayBox->value());
    }
}
} // namespace gui
