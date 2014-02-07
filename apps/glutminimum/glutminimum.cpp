/*********************************************************************************
*
* Inviwo - Interactive Visualization Workshop
* Version 0.6b
*
* Copyright (c) 2012-2014 Inviwo Foundation
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Main file authors: Erik Sund�n, Timo Ropinski
*
*********************************************************************************/

#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <modules/opengl/inviwoopengl.h>
#include <GL/glut.h>
#include <modules/glut/canvasglut.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/network/processornetworkevaluator.h>
#include <inviwo/core/processors/canvasprocessor.h>
#include <inviwo/core/util/filesystem.h>

#include <moduleregistration.h>


using namespace inviwo;

CanvasGLUT* canvas = 0;
InviwoApplication* app = 0;
ProcessorNetwork* processorNetwork = 0;
ProcessorNetworkEvaluator* processorNetworkEvaluator = 0;


void deinitialize() {
    delete processorNetwork;
    processorNetwork = 0;
    delete processorNetworkEvaluator;
    processorNetworkEvaluator = 0;

    if (app) app->deinitialize();

    delete app;
    app = NULL;
    //for(unsigned int i=0; i<CanvasGLUT::canvasCount_; ++i)
    //    delete CanvasGLUT::canvases_[i];
}


void keyPressed(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case 27: // ESC key
            deinitialize();
            exit(0);
            break;
    }
}

void keyPressedSpecial(int /*key*/, int /*x*/, int /*y*/) {
}


int main(int argc, char** argv) {
    InviwoApplication inviwoApp(argc, argv, "Inviwo "+IVW_VERSION + " - GLUTApp", inviwo::filesystem::findBasePath());
    glutInit(&argc, argv);
    canvas = new CanvasGLUT(inviwoApp.getDisplayName(), uvec2(128,128));
    canvas->initializeGL();
    inviwoApp.initialize(&inviwo::registerAllModules);
    // Create process network
    processorNetwork = new ProcessorNetwork();
    inviwoApp.setProcessorNetwork(processorNetwork);
    // Create process network evaluator
    processorNetworkEvaluator = new ProcessorNetworkEvaluator(processorNetwork);
    processorNetworkEvaluator->setDefaultRenderContext(canvas);
    canvas->setNetworkEvaluator(processorNetworkEvaluator);
    canvas->initializeSquare();
    canvas->initialize();
    canvas->activate();
    // Load simple scene
    processorNetwork->lock();
    const CommandLineParser* cmdparser = (inviwo::InviwoApplication::getRef()).getCommandLineParser();
    std::string workspace;

    if (cmdparser->getLoadWorkspaceFromArg())
        workspace = cmdparser->getWorkspacePath();
    else
        workspace = inviwoApp.getPath(InviwoApplication::PATH_WORKSPACES, "tests/simple.inv");

    IvwDeserializer xmlDeserializer(workspace);
    processorNetwork->deserialize(xmlDeserializer);
    std::vector<Processor*> processors = processorNetwork->getProcessors();
    int i=0;

    for (std::vector<Processor*>::iterator it = processors.begin(); it!=processors.end(); it++) {
        (*it)->invalidate(PropertyOwner::INVALID_RESOURCES);
        CanvasProcessor* canvasProcessor = dynamic_cast<CanvasProcessor*>((*it));

        if (canvasProcessor) {
            if (i==0) {
                canvas->setWindowTitle(inviwoApp.getDisplayName() + " : " + canvasProcessor->getIdentifier());
                canvas->setWindowSize(uvec2(canvasProcessor->getCanvasSize()));
                processorNetworkEvaluator->registerCanvas(canvas, canvasProcessor->getIdentifier());
            }
            else {
                CanvasGLUT* newC = new CanvasGLUT(canvasProcessor->getIdentifier(), uvec2(canvasProcessor->getCanvasSize()));
                processorNetworkEvaluator->registerCanvas(newC, canvasProcessor->getIdentifier());
                newC->initializeGL();
                newC->setNetworkEvaluator(processorNetworkEvaluator);
                newC->initialize();
            }

            i++;
        }
    }

    processorNetwork->setModified(true);
    processorNetwork->unlock();
    processorNetwork->modified();

    if (cmdparser->getCaptureAfterStartup()) {
        std::string path = cmdparser->getOutputPath();

        if (path.empty())
            path = InviwoApplication::getPtr()->getPath(InviwoApplication::PATH_IMAGES);

        processorNetworkEvaluator->saveSnapshotAllCanvases(path, cmdparser->getSnapshotName());
    }

    if (cmdparser->getQuitApplicationAfterStartup())
        return 0;

    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(keyPressedSpecial);
    glutMainLoop();
    return 0;
}
