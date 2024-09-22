project(AnimeEffects)

set(MACOSX_BUNDLE_ICON_FILE AnimeEffects.icns)	

if(WIN32)
    set(app_icon "icon.rc")
elseif(APPLE)
    set(app_icon "../AnimeEffects.icns")
    set_source_files_properties(${app_icon} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
elseif(UNIX)
    file(COPY ${CMAKE_SOURCE_DIR}/dist/AnimeEffects.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps/)
    file(COPY ${CMAKE_SOURCE_DIR}/dist/AnimeEffects.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/)
    file(COPY ${CMAKE_SOURCE_DIR}/dist/AnimeEffects.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable)
    file(COPY ${CMAKE_SOURCE_DIR}/dist/AnimeEffects.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications/)
    file(COPY ${CMAKE_SOURCE_DIR}/dist/AnimeEffects.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications/org.anie.AnimeEffects)
    
endif()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    ../XC.h
    ../XCAssert.h
    ../XCReport.h
    ../XCType.h
    CanvasMover.cpp CanvasMover.h
    DriverHolder.cpp DriverHolder.h
    EasyDialog.cpp EasyDialog.h
    ExportDialog.cpp ExportDialog.h
    GUIResources.cpp GUIResources.h
    GeneralSettingDialog.cpp GeneralSettingDialog.h
    KeyBindingDialog.cpp KeyBindingDialog.h
    KeyCommandInvoker.cpp KeyCommandInvoker.h
    KeyCommandMap.cpp KeyCommandMap.h
    LocaleDecider.cpp LocaleDecider.h
    LocaleParam.h
    MSVCBackTracer.cpp MSVCBackTracer.h
    MSVCMemoryLeakDebugger.cpp MSVCMemoryLeakDebugger.h
    Main.cpp
    MainDisplayMode.h
    MainDisplayStyle.h
    MainDisplayWidget.cpp MainDisplayWidget.h
    MainMenuBar.cpp MainMenuBar.h
    MainViewSetting.h
    MainWindow.cpp MainWindow.h
    MouseSetting.cpp MouseSetting.h
    MouseSettingDialog.cpp MouseSettingDialog.h
    NewProjectDialog.cpp NewProjectDialog.h
    ObjectTreeWidget.cpp ObjectTreeWidget.h
    PlayBackWidget.cpp PlayBackWidget.h
    ProjectHook.cpp ProjectHook.h
    ProjectTabBar.cpp ProjectTabBar.h
    PropertyWidget.cpp PropertyWidget.h
    ResourceDialog.cpp ResourceDialog.h
    ResourceTreeWidget.cpp ResourceTreeWidget.h
    TargetWidget.cpp TargetWidget.h
    TimeLineEditorWidget.cpp TimeLineEditorWidget.h
    TimeLineInfoWidget.cpp TimeLineInfoWidget.h
    TimeLineWidget.cpp TimeLineWidget.h
    ToolWidget.cpp ToolWidget.h
    ViaPoint.cpp ViaPoint.h
    exportdiag.cpp exportdiag.h
    menu/menu_ProgressReporter.cpp menu/menu_ProgressReporter.h
    obj/obj_InsertItem.cpp obj/obj_InsertItem.h
    obj/obj_Item.cpp obj/obj_Item.h
    obj/obj_MoveItem.cpp obj/obj_MoveItem.h
    obj/obj_Notifiers.cpp obj/obj_Notifiers.h
    obj/obj_RemoveItem.cpp obj/obj_RemoveItem.h
    obj/obj_Util.cpp obj/obj_Util.h
    prop/prop_AttrGroup.cpp prop/prop_AttrGroup.h
    prop/prop_Backboard.cpp prop/prop_Backboard.h
    prop/prop_ConstantPanel.cpp prop/prop_ConstantPanel.h
    prop/prop_CurrentKeyPanel.cpp prop/prop_CurrentKeyPanel.h
    prop/prop_DefaultKeyPanel.cpp prop/prop_DefaultKeyPanel.h
    prop/prop_ItemBase.h
    prop/prop_Items.cpp prop/prop_Items.h
    prop/prop_KeyAccessor.cpp prop/prop_KeyAccessor.h
    prop/prop_KeyGroup.cpp prop/prop_KeyGroup.h
    prop/prop_KeyKnocker.cpp prop/prop_KeyKnocker.h
    prop/prop_Panel.cpp prop/prop_Panel.h
    prop/prop_ProjectPanel.cpp prop/prop_ProjectPanel.h
    res/res_ImageSetter.h
    res/res_Item.cpp res/res_Item.h
    res/res_Notifier.cpp res/res_Notifier.h
    res/res_ResourceUpdater.cpp res/res_ResourceUpdater.h
    theme/Theme.cpp theme/Theme.h
    theme/TimeLine.cpp theme/TimeLine.h
    tool/tool_BonePanel.cpp tool/tool_BonePanel.h
    tool/tool_FFDPanel.cpp tool/tool_FFDPanel.h
    tool/tool_FlowLayout.cpp tool/tool_FlowLayout.h
    tool/tool_ItemTable.cpp tool/tool_ItemTable.h
    tool/tool_Items.cpp tool/tool_Items.h
    tool/tool_MeshPanel.cpp tool/tool_MeshPanel.h
    tool/tool_ModePanel.cpp tool/tool_ModePanel.h
    tool/tool_PosePanel.cpp tool/tool_PosePanel.h
    tool/tool_SRTPanel.cpp tool/tool_SRTPanel.h
    tool/tool_ViewPanel.cpp tool/tool_ViewPanel.h
    ${app_icon}
        AudioPlaybackWidget.cpp
        AudioPlaybackWidget.h
)

b_embed(${PROJECT_NAME} ../../data/encode/VideoEncode.txt)
b_embed(${PROJECT_NAME} ../../data/locale/translation_ja.qm)
b_embed(${PROJECT_NAME} ../../data/locale/translation_zh.qm)
b_embed(${PROJECT_NAME} ../../data/locale/preference.txt)
b_embed(${PROJECT_NAME} ../../data/locale/preference_ja.txt)
b_embed(${PROJECT_NAME} ../../data/locale/preference_zh.txt)

target_include_directories(${PROJECT_NAME} PRIVATE
    ..
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    AE_MAJOR_VERSION=1
    AE_MICRO_VERSION=0
    AE_MINOR_VERSION=6
    AE_PROJECT_FORMAT_MAJOR_VERSION=0
    AE_PROJECT_FORMAT_MINOR_VERSION=7
    AE_PROJECT_FORMAT_OLDEST_MAJOR_VERSION=0
    AE_PROJECT_FORMAT_OLDEST_MINOR_VERSION=4
    USE_GL_CORE_PROFILE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core Qt6::Core5Compat Qt6::Widgets
        Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets
        Qt6::Xml Qt6::Concurrent Qt6::Multimedia
        cmnd core ctrl gl img thr util
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_target(copy_data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
add_dependencies(${PROJECT_NAME} copy_data)
