#############################################################################
# Makefile for building: qt_widgets.app/Contents/MacOS/qt_widgets
# Generated by qmake (2.01a) (Qt 4.8.6) on: Fri Oct 24 16:27:38 2014
# Project:  qt_widgets.pro
# Template: app
# Command: /usr/local/bin/qmake -spec /usr/local/Cellar/qt/4.8.5/mkspecs/macx-xcode -o qt_widgets.xcodeproj/project.pbxproj qt_widgets.pro
#############################################################################

MOC       = /usr/local/Cellar/qt/4.8.5/bin/moc
UIC       = /usr/local/Cellar/qt/4.8.5/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Cellar/qt/4.8.5/mkspecs/macx-xcode -I. -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtOpenGL.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtOpenGL.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/include -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/System/Library/Frameworks/AGL.framework/Headers -I. -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/usr/local/Cellar/qt/4.8.5/lib
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: ./moc_scenewidget.cpp ./moc_imagewidget.cpp ./moc_heatmapwidget.cpp\
	 ./moc_window.cpp ./moc_pcwidget.cpp ./moc_renderwidget.cpp
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc_scenewidget.cpp moc_imagewidget.cpp moc_heatmapwidget.cpp moc_window.cpp moc_pcwidget.cpp moc_renderwidget.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_scenewidget.cpp moc_imagewidget.cpp moc_heatmapwidget.cpp moc_window.cpp moc_pcwidget.cpp moc_renderwidget.cpp
moc_scenewidget.cpp: RIV/Views/3DView.h \
		RIV/Views/DataView.h \
		RIV/Data/DataSet.h \
		RIV/Data/Table.h \
		RIV/Data/Filter.h \
		RIV/Data/Reference.h \
		RIV/Data/Record.h \
		RIV/Data/Iterator.h \
		RIV/Data/Cluster.h \
		RIV/helper.h \
		RIV/Geometry/Geometry.h \
		RIV/Data/ClusterSet.h \
		RIV/Graphics/Evaluator.h \
		RIV/Data/DataSetListener.h \
		RIV/reporter.h \
		RIV/Graphics/ColorProperty.h \
		RIV/Graphics/Property.h \
		RIV/Graphics/ColorPalette.h \
		RIV/Graphics/ColorMap.h \
		RIV/Graphics/SizeProperty.h \
		RIV/PBRTConfig.h \
		RIV/Geometry/Path.h \
		RIV/Geometry/MeshModel.h \
		RIV/Geometry/TriangleMesh.h \
		RIV/Geometry/Vec3D.h \
		RIV/Geometry/Ray.h \
		RIV/Octree/Octree.h \
		RIV/Octree/OctreeNode.h \
		RIV/Octree/OctreeConfig.h \
		scenewidget.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ scenewidget.h -o moc_scenewidget.cpp

moc_imagewidget.cpp: RIV/Views/ImageView.h \
		RIV/Views/DataView.h \
		RIV/Data/DataSet.h \
		RIV/Data/Table.h \
		RIV/Data/Filter.h \
		RIV/Data/Reference.h \
		RIV/Data/Record.h \
		RIV/Data/Iterator.h \
		RIV/Data/Cluster.h \
		RIV/helper.h \
		RIV/Geometry/Geometry.h \
		RIV/Data/ClusterSet.h \
		RIV/Graphics/Evaluator.h \
		RIV/Data/DataSetListener.h \
		RIV/reporter.h \
		RIV/Graphics/ColorProperty.h \
		RIV/Graphics/Property.h \
		RIV/Graphics/ColorPalette.h \
		RIV/Graphics/ColorMap.h \
		RIV/Graphics/SizeProperty.h \
		RIV/Graphics/BMPImage.h \
		imagewidget.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ imagewidget.h -o moc_imagewidget.cpp

moc_heatmapwidget.cpp: RIV/Views/HeatMapView.h \
		RIV/Views/DataView.h \
		RIV/Data/DataSet.h \
		RIV/Data/Table.h \
		RIV/Data/Filter.h \
		RIV/Data/Reference.h \
		RIV/Data/Record.h \
		RIV/Data/Iterator.h \
		RIV/Data/Cluster.h \
		RIV/helper.h \
		RIV/Geometry/Geometry.h \
		RIV/Data/ClusterSet.h \
		RIV/Graphics/Evaluator.h \
		RIV/Data/DataSetListener.h \
		RIV/reporter.h \
		RIV/Graphics/ColorProperty.h \
		RIV/Graphics/Property.h \
		RIV/Graphics/ColorPalette.h \
		RIV/Graphics/ColorMap.h \
		RIV/Graphics/SizeProperty.h \
		heatmapwidget.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ heatmapwidget.h -o moc_heatmapwidget.cpp

moc_window.cpp: RIV/PBRTConfig.h \
		imagewidget.h \
		RIV/Views/ImageView.h \
		RIV/Views/DataView.h \
		RIV/Data/DataSet.h \
		RIV/Data/Table.h \
		RIV/Data/Filter.h \
		RIV/Data/Reference.h \
		RIV/Data/Record.h \
		RIV/Data/Iterator.h \
		RIV/Data/Cluster.h \
		RIV/helper.h \
		RIV/Geometry/Geometry.h \
		RIV/Data/ClusterSet.h \
		RIV/Graphics/Evaluator.h \
		RIV/Data/DataSetListener.h \
		RIV/reporter.h \
		RIV/Graphics/ColorProperty.h \
		RIV/Graphics/Property.h \
		RIV/Graphics/ColorPalette.h \
		RIV/Graphics/ColorMap.h \
		RIV/Graphics/SizeProperty.h \
		RIV/Graphics/BMPImage.h \
		heatmapwidget.h \
		RIV/Views/HeatMapView.h \
		pcwidget.h \
		RIV/Views/ParallelCoordsView.h \
		RIV/Views/ParallelCoordsAxis.h \
		RIV/Views/ParallelCoordsAxisGroup.h \
		scenewidget.h \
		RIV/Views/3DView.h \
		RIV/Geometry/Path.h \
		RIV/Geometry/MeshModel.h \
		RIV/Geometry/TriangleMesh.h \
		RIV/Geometry/Vec3D.h \
		RIV/Geometry/Ray.h \
		RIV/Octree/Octree.h \
		RIV/Octree/OctreeNode.h \
		RIV/Octree/OctreeConfig.h \
		window.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ window.h -o moc_window.cpp

moc_pcwidget.cpp: RIV/Views/ParallelCoordsView.h \
		RIV/Views/DataView.h \
		RIV/Data/DataSet.h \
		RIV/Data/Table.h \
		RIV/Data/Filter.h \
		RIV/Data/Reference.h \
		RIV/Data/Record.h \
		RIV/Data/Iterator.h \
		RIV/Data/Cluster.h \
		RIV/helper.h \
		RIV/Geometry/Geometry.h \
		RIV/Data/ClusterSet.h \
		RIV/Graphics/Evaluator.h \
		RIV/Data/DataSetListener.h \
		RIV/reporter.h \
		RIV/Graphics/ColorProperty.h \
		RIV/Graphics/Property.h \
		RIV/Graphics/ColorPalette.h \
		RIV/Graphics/ColorMap.h \
		RIV/Graphics/SizeProperty.h \
		RIV/Views/ParallelCoordsAxis.h \
		RIV/Views/ParallelCoordsAxisGroup.h \
		pcwidget.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ pcwidget.h -o moc_pcwidget.cpp

moc_renderwidget.cpp: renderwidget.h
	/usr/local/Cellar/qt/4.8.5/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ renderwidget.h -o moc_renderwidget.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

