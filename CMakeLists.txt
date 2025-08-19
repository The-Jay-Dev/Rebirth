cmake_minimum_required(VERSION 3.16)

project(Rebirth VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.8)
 set(app_icon_resource_windows  "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

qt_add_executable(appRebirth
    main.cpp
    ${app_icon_resource_windows}
)


qt_add_qml_module(appRebirth
    URI Rebirth
    VERSION 1.0

    QML_FILES
    Main.qml
    GameplayUI.qml
    HitObject.qml
    CustomFonts.qml
    NotificationMessage.qml
    JudgementLine.qml
    Trail.qml
    MapSelectMenu.qml
    MapSelectSideMenu.qml
    MapSelectSideMenuOptionBtn.qml
    MapSelectTransition.qml
    TitleScreen.qml
    MapBackground.qml
    Results.qml
    EmotionalCapacity.qml
    HoldNoteController.qml
    DoubleTapJudgementRing.qml
    NoteShatterEffect.qml
    QmlSoundEffects.qml
    Stage.qml

    SOURCES
    MusicConductor.h
    MusicConductor.cpp
    miniaudio.h
    GameManager.h
    GameManager.cpp
    TimeStep.h
    Notes.h
    GameInitializer.h
    GameInitializer.cpp
    HitObjectModel.h
    HitObjectModel.cpp
    MapFileHandler.h
    MapFileHandler.cpp
    MapData.h
    JudgementLineModel.h
    JudgementLineModel.cpp
    EmotionalCapacityStates.h
    DataHandlerModel.h
    DataHandlerModel.cpp
    HitObject.h
    AudioHandler.h
    AudioHandler.cpp
    HitObjectEnums.h
    MessageHandler.h
    NotePool.h
    NotePool.cpp

    RESOURCES
    Resource.qrc
    UI/EmotionalCapacity.png
    UI/S.png
    UI/A.png
    UI/B.png
    UI/C.png
    UI/D.png
    UI/Note.png
    UI/ForgottenNote.png
    UI/NoteEndLeft.png
    UI/NoteEndRight.png
    UI/refresh-outline.svg
    UI/chevron-back-sharp.svg
    UI/alert-circle-outline.svg
    UI/Rebirth.png
    UI/TitleCardBG.png
    UI/RebirthDiamond.png
    UI/NoteBaseNeutral.png
    UI/Recovered.png
    UI/Forgotten.png
    UI/Preserved.png
    UI/Fracture.png
    UI/Euphoria.png
    UI/Unknown.png
    UI/Nostalgia.png
    UI/Paradox.png
    UI/Confusion.png
    UI/Bliss.png
    UI/chevron-back-circle-outline.svg
    Particles/ParticleDiamond.png
    Particles/ParticleShockwavePerfect.png
    Particles/ParticleBase.png
    Particles/MemoryParticle.png
    Particles/ParticleShockwave.png
    Particles/ParticleShatter.png
    Fonts/Barlow-Black.ttf
    Fonts/Barlow-Light.ttf
    Fonts/Barlow-LightItalic.ttf
    Fonts/Barlow-Regular.ttf

)

qt_add_resources(appRebirth ""
PREFIX "/"
FILES
RebirthIcon.ico
)





# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appRebirth PROPERTIES
    #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appRebirth
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appRebirth
    PRIVATE Qt6::Quick Qt6::Concurrent
)


include(GNUInstallDirs)
install(TARGETS appRebirth
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


