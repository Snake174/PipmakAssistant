QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += Qt5

CONFIG += qt warn_on thread resources l10n

OBJECTS_DIR = obj
MOC_DIR = obj
RCC_DIR = obj

DEFINES += APP_VERSION=\\\"0.7\\\"
DEFINES += SCINTILLA_QT SCI_LEXER
DEFINES += GL_GLEXT_PROTOTYPES

win32: RC_FILE = ICON.rc

win32: DEFINES += WIN32
win32: DEFINES += _USE_MATH_DEFINES

win32: LIBS += -lopengl32
win32: LIBS += -Llibs/WIN32/VC2010/OpenAL-1.15.1/lib -lOpenAL32
win32: LIBS += -Llibs/WIN32/VC2010/libogg-1.3.1/lib -llibogg
win32: LIBS += -Llibs/WIN32/VC2010/libvorbis-1.3.4/lib -llibvorbis
win32: LIBS += -Llibs/WIN32/VC2010/libvorbis-1.3.4/lib -llibvorbisfile

win32: INCLUDEPATH += libs/WIN32/VC2010/OpenAL-1.15.1/include libs/WIN32/VC2010/libogg-1.3.1/include libs/WIN32/VC2010/libvorbis-1.3.4/include

unix: LIBS += -lGL -lopenal
unix: LIBS += -Llibs/UNIX/libogg-1.3.1/lib -logg
unix: LIBS += -Llibs/UNIX/libvorbis-1.3.4/lib -lvorbis
unix: LIBS += -Llibs/UNIX/libvorbis-1.3.4/lib -lvorbisfile

unix: INCLUDEPATH += libs/UNIX/libogg-1.3.1/include libs/UNIX/libvorbis-1.3.4/include

INCLUDEPATH += libs/zlib123
INCLUDEPATH += libs/editor/Qt4Qt5

TARGET = PipmakAssistant
TEMPLATE = app

SOURCES += \
  main.cpp \
  MainWindow.cpp \
  Project/ProjectList.cpp \
  PathUtils.cpp \
  Project/ProjectTree.cpp \
  LoadingEffect.cpp \
  PipmakResource/PipmakResource.cpp \
  PipmakResource/zipglobal.cpp \
  PipmakResource/zip.cpp \
  PipmakResource/unzip.cpp \
  Settings.cpp \
  LuaParser/LuaParser.cpp \
  Globals.cpp \
  Node/Node.cpp \
  Project/ProjectLegend.cpp \
  Project/LegendScene.cpp \
  Node/HotSpotEditor.cpp \
  ScriptEditor/ScriptViewer.cpp \
  ScriptEditor/ScriptEditor.cpp \
  Node/PatchEditor.cpp \
  Node/PatchViewer.cpp \
  SettingsDialog/SettingsDialog.cpp \
  Node/CubeMapViewer.cpp \
  PropertyEditor/Property.cpp \
  Node/SceneEditor.cpp \
  Node/SlideViewer.cpp \
  Node/PanelViewer.cpp \
  About.cpp \
  wigglywidget.cpp \
  libs/zlib123/zutil.c \
  libs/zlib123/uncompr.c \
  libs/zlib123/trees.c \
  libs/zlib123/inftrees.c \
  libs/zlib123/inflate.c \
  libs/zlib123/inffast.c \
  libs/zlib123/infback.c \
  libs/zlib123/gzio.c \
  libs/zlib123/deflate.c \
  libs/zlib123/crc32.c \
  libs/zlib123/compress.c \
  libs/zlib123/adler32.c \
  libs/editor/lexers/LexYAML.cpp \
  libs/editor/lexers/LexVisualProlog.cpp \
  libs/editor/lexers/LexVHDL.cpp \
  libs/editor/lexers/LexVerilog.cpp \
  libs/editor/lexers/LexVB.cpp \
  libs/editor/lexers/LexTxt2tags.cpp \
  libs/editor/lexers/LexTeX.cpp \
  libs/editor/lexers/LexTCMD.cpp \
  libs/editor/lexers/LexTCL.cpp \
  libs/editor/lexers/LexTAL.cpp \
  libs/editor/lexers/LexTADS3.cpp \
  libs/editor/lexers/LexTACL.cpp \
  libs/editor/lexers/LexSTTXT.cpp \
  libs/editor/lexers/LexSQL.cpp \
  libs/editor/lexers/LexSpice.cpp \
  libs/editor/lexers/LexSpecman.cpp \
  libs/editor/lexers/LexSorcus.cpp \
  libs/editor/lexers/LexSML.cpp \
  libs/editor/lexers/LexSmalltalk.cpp \
  libs/editor/lexers/LexScriptol.cpp \
  libs/editor/lexers/LexRust.cpp \
  libs/editor/lexers/LexRuby.cpp \
  libs/editor/lexers/LexRebol.cpp \
  libs/editor/lexers/LexR.cpp \
  libs/editor/lexers/LexPython.cpp \
  libs/editor/lexers/LexPS.cpp \
  libs/editor/lexers/LexProgress.cpp \
  libs/editor/lexers/LexPowerShell.cpp \
  libs/editor/lexers/LexPowerPro.cpp \
  libs/editor/lexers/LexPOV.cpp \
  libs/editor/lexers/LexPO.cpp \
  libs/editor/lexers/LexPLM.cpp \
  libs/editor/lexers/LexPerl.cpp \
  libs/editor/lexers/LexPB.cpp \
  libs/editor/lexers/LexPascal.cpp \
  libs/editor/lexers/LexOthers.cpp \
  libs/editor/lexers/LexOScript.cpp \
  libs/editor/lexers/LexOpal.cpp \
  libs/editor/lexers/LexNsis.cpp \
  libs/editor/lexers/LexNimrod.cpp \
  libs/editor/lexers/LexMySQL.cpp \
  libs/editor/lexers/LexMSSQL.cpp \
  libs/editor/lexers/LexMPT.cpp \
  libs/editor/lexers/LexModula.cpp \
  libs/editor/lexers/LexMMIXAL.cpp \
  libs/editor/lexers/LexMetapost.cpp \
  libs/editor/lexers/LexMatlab.cpp \
  libs/editor/lexers/LexMarkdown.cpp \
  libs/editor/lexers/LexMagik.cpp \
  libs/editor/lexers/LexLua.cpp \
  libs/editor/lexers/LexLout.cpp \
  libs/editor/lexers/LexLisp.cpp \
  libs/editor/lexers/LexLaTeX.cpp \
  libs/editor/lexers/LexKVIrc.cpp \
  libs/editor/lexers/LexKix.cpp \
  libs/editor/lexers/LexInno.cpp \
  libs/editor/lexers/LexHTML.cpp \
  libs/editor/lexers/LexHaskell.cpp \
  libs/editor/lexers/LexGui4Cli.cpp \
  libs/editor/lexers/LexGAP.cpp \
  libs/editor/lexers/LexFortran.cpp \
  libs/editor/lexers/LexForth.cpp \
  libs/editor/lexers/LexFlagship.cpp \
  libs/editor/lexers/LexEScript.cpp \
  libs/editor/lexers/LexErlang.cpp \
  libs/editor/lexers/LexEiffel.cpp \
  libs/editor/lexers/LexECL.cpp \
  libs/editor/lexers/LexD.cpp \
  libs/editor/lexers/LexCSS.cpp \
  libs/editor/lexers/LexCsound.cpp \
  libs/editor/lexers/LexCrontab.cpp \
  libs/editor/lexers/LexCPP.cpp \
  libs/editor/lexers/LexConf.cpp \
  libs/editor/lexers/LexCoffeeScript.cpp \
  libs/editor/lexers/LexCOBOL.cpp \
  libs/editor/lexers/LexCmake.cpp \
  libs/editor/lexers/LexCLW.cpp \
  libs/editor/lexers/LexCaml.cpp \
  libs/editor/lexers/LexBullant.cpp \
  libs/editor/lexers/LexBasic.cpp \
  libs/editor/lexers/LexBash.cpp \
  libs/editor/lexers/LexBaan.cpp \
  libs/editor/lexers/LexAVS.cpp \
  libs/editor/lexers/LexAVE.cpp \
  libs/editor/lexers/LexAU3.cpp \
  libs/editor/lexers/LexASY.cpp \
  libs/editor/lexers/LexAsn1.cpp \
  libs/editor/lexers/LexAsm.cpp \
  libs/editor/lexers/LexAPDL.cpp \
  libs/editor/lexers/LexAda.cpp \
  libs/editor/lexers/LexAbaqus.cpp \
  libs/editor/lexers/LexA68k.cpp \
  libs/editor/lexlib/WordList.cpp \
  libs/editor/lexlib/StyleContext.cpp \
  libs/editor/lexlib/PropSetSimple.cpp \
  libs/editor/lexlib/LexerSimple.cpp \
  libs/editor/lexlib/LexerNoExceptions.cpp \
  libs/editor/lexlib/LexerModule.cpp \
  libs/editor/lexlib/LexerBase.cpp \
  libs/editor/lexlib/CharacterSet.cpp \
  libs/editor/lexlib/CharacterCategory.cpp \
  libs/editor/lexlib/Accessor.cpp \
  libs/editor/Qt4Qt5/ScintillaQt.cpp \
  libs/editor/Qt4Qt5/SciClasses.cpp \
  libs/editor/Qt4Qt5/qscistyledtext.cpp \
  libs/editor/Qt4Qt5/qscistyle.cpp \
  libs/editor/Qt4Qt5/qsciscintillabase.cpp \
  libs/editor/Qt4Qt5/qsciscintilla.cpp \
  libs/editor/Qt4Qt5/qsciprinter.cpp \
  libs/editor/Qt4Qt5/qscimacro.cpp \
  libs/editor/Qt4Qt5/qscilexeryaml.cpp \
  libs/editor/Qt4Qt5/qscilexerxml.cpp \
  libs/editor/Qt4Qt5/qscilexervhdl.cpp \
  libs/editor/Qt4Qt5/qscilexerverilog.cpp \
  libs/editor/Qt4Qt5/qscilexertex.cpp \
  libs/editor/Qt4Qt5/qscilexertcl.cpp \
  libs/editor/Qt4Qt5/qscilexersql.cpp \
  libs/editor/Qt4Qt5/qscilexerspice.cpp \
  libs/editor/Qt4Qt5/qscilexerruby.cpp \
  libs/editor/Qt4Qt5/qscilexerpython.cpp \
  libs/editor/Qt4Qt5/qscilexerproperties.cpp \
  libs/editor/Qt4Qt5/qscilexerpov.cpp \
  libs/editor/Qt4Qt5/qscilexerpostscript.cpp \
  libs/editor/Qt4Qt5/qscilexerperl.cpp \
  libs/editor/Qt4Qt5/qscilexerpascal.cpp \
  libs/editor/Qt4Qt5/qscilexeroctave.cpp \
  libs/editor/Qt4Qt5/qscilexermatlab.cpp \
  libs/editor/Qt4Qt5/qscilexermakefile.cpp \
  libs/editor/Qt4Qt5/qscilexerlua.cpp \
  libs/editor/Qt4Qt5/qscilexerjavascript.cpp \
  libs/editor/Qt4Qt5/qscilexerjava.cpp \
  libs/editor/Qt4Qt5/qscilexeridl.cpp \
  libs/editor/Qt4Qt5/qscilexerhtml.cpp \
  libs/editor/Qt4Qt5/qscilexerfortran77.cpp \
  libs/editor/Qt4Qt5/qscilexerfortran.cpp \
  libs/editor/Qt4Qt5/qscilexerdiff.cpp \
  libs/editor/Qt4Qt5/qscilexerd.cpp \
  libs/editor/Qt4Qt5/qscilexercustom.cpp \
  libs/editor/Qt4Qt5/qscilexercss.cpp \
  libs/editor/Qt4Qt5/qscilexercsharp.cpp \
  libs/editor/Qt4Qt5/qscilexercpp.cpp \
  libs/editor/Qt4Qt5/qscilexercmake.cpp \
  libs/editor/Qt4Qt5/qscilexerbatch.cpp \
  libs/editor/Qt4Qt5/qscilexerbash.cpp \
  libs/editor/Qt4Qt5/qscilexer.cpp \
  libs/editor/Qt4Qt5/qscidocument.cpp \
  libs/editor/Qt4Qt5/qscicommandset.cpp \
  libs/editor/Qt4Qt5/qscicommand.cpp \
  libs/editor/Qt4Qt5/qsciapis.cpp \
  libs/editor/Qt4Qt5/qsciabstractapis.cpp \
  libs/editor/Qt4Qt5/PlatQt.cpp \
  libs/editor/Qt4Qt5/MacPasteboardMime.cpp \
  libs/editor/Qt4Qt5/ListBoxQt.cpp \
  libs/editor/Qt4Qt5/InputMethod.cpp \
  libs/editor/src/XPM.cpp \
  libs/editor/src/ViewStyle.cpp \
  libs/editor/src/UniConversion.cpp \
  libs/editor/src/Style.cpp \
  libs/editor/src/Selection.cpp \
  libs/editor/src/ScintillaBase.cpp \
  libs/editor/src/RunStyles.cpp \
  libs/editor/src/RESearch.cpp \
  libs/editor/src/PositionCache.cpp \
  libs/editor/src/PerLine.cpp \
  libs/editor/src/LineMarker.cpp \
  libs/editor/src/KeyMap.cpp \
  libs/editor/src/Indicator.cpp \
  libs/editor/src/ExternalLexer.cpp \
  libs/editor/src/Editor.cpp \
  libs/editor/src/Document.cpp \
  libs/editor/src/Decoration.cpp \
  libs/editor/src/ContractionState.cpp \
  libs/editor/src/CharClassify.cpp \
  libs/editor/src/CellBuffer.cpp \
  libs/editor/src/Catalogue.cpp \
  libs/editor/src/CaseFolder.cpp \
  libs/editor/src/CaseConvert.cpp \
  libs/editor/src/CallTip.cpp \
  libs/editor/src/AutoComplete.cpp \
  Sound/ALApp.cpp \
  Sound/AudioPlayer.cpp \
  Node/HotSpotViewer.cpp \
  Sound/OggFile.cpp \
    CodeGenerator/CodeGenerator.cpp

HEADERS += \
  MainWindow.h \
  Project/ProjectList.h \
  PathUtils.h \
  Project/ProjectTree.h \
  Structures.h \
  LoadingEffect.h \
  PipmakResource/PipmakResource.h \
  PipmakResource/zip_p.h \
  PipmakResource/zipglobal.h \
  PipmakResource/zipentry_p.h \
  PipmakResource/zip.h \
  PipmakResource/unzip_p.h \
  PipmakResource/unzip.h \
  Singleton.h \
  Settings.h \
  call_once.h \
  LuaParser/LuaParser.h \
  Globals.h \
  Node/Node.h \
  Project/ProjectLegend.h \
  Project/LegendScene.h \
  Project/ColorPalette.h \
  Node/HotSpotEditor.h \
  ScriptEditor/ScriptViewer.h \
  ScriptEditor/ScriptEditor.h \
  Node/PatchEditor.h \
  Node/PatchViewer.h \
  SettingsDialog/SettingsDialog.h \
  Node/CubeMapViewer.h \
  PropertyEditor/Property.h \
  Node/SceneEditor.h \
  Node/SlideViewer.h \
  Node/PanelViewer.h \
  About.h \
  wigglywidget.h \
  libs/zlib123/zutil.h \
  libs/zlib123/zlib.h \
  libs/zlib123/zconf.h \
  libs/zlib123/trees.h \
  libs/zlib123/inftrees.h \
  libs/zlib123/inflate.h \
  libs/zlib123/inffixed.h \
  libs/zlib123/inffast.h \
  libs/zlib123/deflate.h \
  libs/zlib123/crc32.h \
  libs/editor/include/ScintillaWidget.h \
  libs/editor/include/Scintilla.h \
  libs/editor/include/SciLexer.h \
  libs/editor/include/Platform.h \
  libs/editor/include/ILexer.h \
  libs/editor/lexlib/WordList.h \
  libs/editor/lexlib/SubStyles.h \
  libs/editor/lexlib/StyleContext.h \
  libs/editor/lexlib/SparseState.h \
  libs/editor/lexlib/PropSetSimple.h \
  libs/editor/lexlib/OptionSet.h \
  libs/editor/lexlib/LexerSimple.h \
  libs/editor/lexlib/LexerNoExceptions.h \
  libs/editor/lexlib/LexerModule.h \
  libs/editor/lexlib/LexerBase.h \
  libs/editor/lexlib/LexAccessor.h \
  libs/editor/lexlib/CharacterSet.h \
  libs/editor/lexlib/CharacterCategory.h \
  libs/editor/lexlib/Accessor.h \
  libs/editor/Qt4Qt5/ScintillaQt.h \
  libs/editor/Qt4Qt5/SciNamespace.h \
  libs/editor/Qt4Qt5/SciClasses.h \
  libs/editor/Qt4Qt5/ListBoxQt.h \
  libs/editor/Qt4Qt5/Qsci/qscistyledtext.h \
  libs/editor/Qt4Qt5/Qsci/qscistyle.h \
  libs/editor/Qt4Qt5/Qsci/qsciscintillabase.h \
  libs/editor/Qt4Qt5/Qsci/qsciscintilla.h \
  libs/editor/Qt4Qt5/Qsci/qsciprinter.h \
  libs/editor/Qt4Qt5/Qsci/qscimacro.h \
  libs/editor/Qt4Qt5/Qsci/qscilexeryaml.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerxml.h \
  libs/editor/Qt4Qt5/Qsci/qscilexervhdl.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerverilog.h \
  libs/editor/Qt4Qt5/Qsci/qscilexertex.h \
  libs/editor/Qt4Qt5/Qsci/qscilexertcl.h \
  libs/editor/Qt4Qt5/Qsci/qscilexersql.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerspice.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerruby.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerpython.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerproperties.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerpov.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerpostscript.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerperl.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerpascal.h \
  libs/editor/Qt4Qt5/Qsci/qscilexeroctave.h \
  libs/editor/Qt4Qt5/Qsci/qscilexermatlab.h \
  libs/editor/Qt4Qt5/Qsci/qscilexermakefile.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerlua.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerjavascript.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerjava.h \
  libs/editor/Qt4Qt5/Qsci/qscilexeridl.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerhtml.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerfortran77.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerfortran.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerdiff.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerd.h \
  libs/editor/Qt4Qt5/Qsci/qscilexercustom.h \
  libs/editor/Qt4Qt5/Qsci/qscilexercss.h \
  libs/editor/Qt4Qt5/Qsci/qscilexercsharp.h \
  libs/editor/Qt4Qt5/Qsci/qscilexercpp.h \
  libs/editor/Qt4Qt5/Qsci/qscilexercmake.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerbatch.h \
  libs/editor/Qt4Qt5/Qsci/qscilexerbash.h \
  libs/editor/Qt4Qt5/Qsci/qscilexer.h \
  libs/editor/Qt4Qt5/Qsci/qsciglobal.h \
  libs/editor/Qt4Qt5/Qsci/qscidocument.h \
  libs/editor/Qt4Qt5/Qsci/qscicommandset.h \
  libs/editor/Qt4Qt5/Qsci/qscicommand.h \
  libs/editor/Qt4Qt5/Qsci/qsciapis.h \
  libs/editor/Qt4Qt5/Qsci/qsciabstractapis.h \
  libs/editor/src/XPM.h \
  libs/editor/src/ViewStyle.h \
  libs/editor/src/UniConversion.h \
  libs/editor/src/UnicodeFromUTF8.h \
  libs/editor/src/Style.h \
  libs/editor/src/SplitVector.h \
  libs/editor/src/Selection.h \
  libs/editor/src/ScintillaBase.h \
  libs/editor/src/RunStyles.h \
  libs/editor/src/RESearch.h \
  libs/editor/src/PositionCache.h \
  libs/editor/src/PerLine.h \
  libs/editor/src/Partitioning.h \
  libs/editor/src/LineMarker.h \
  libs/editor/src/KeyMap.h \
  libs/editor/src/Indicator.h \
  libs/editor/src/FontQuality.h \
  libs/editor/src/ExternalLexer.h \
  libs/editor/src/Editor.h \
  libs/editor/src/Document.h \
  libs/editor/src/Decoration.h \
  libs/editor/src/ContractionState.h \
  libs/editor/src/CharClassify.h \
  libs/editor/src/CellBuffer.h \
  libs/editor/src/Catalogue.h \
  libs/editor/src/CaseFolder.h \
  libs/editor/src/CaseConvert.h \
  libs/editor/src/CallTip.h \
  libs/editor/src/AutoComplete.h \
  Sound/ALApp.h \
  Sound/AudioPlayer.h \
  Node/HotSpotViewer.h \
  Sound/OggFile.h \
    CodeGenerator/CodeGenerator.h

RESOURCES += res.qrc
