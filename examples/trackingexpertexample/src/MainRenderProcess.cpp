#include "MainRenderProcess.h"



MainRenderProcess* MainRenderProcess::m_instance = nullptr;

//static 
MainRenderProcess* MainRenderProcess::getInstance()
{
	if (m_instance == nullptr) {
		
		m_instance = new MainRenderProcess();
	}
	return m_instance;
}

MainRenderProcess::~MainRenderProcess()
{
	delete gl_camera_point_cloud;
	delete gl_reference_point_cloud;
	delete gl_reference_eval;

	// render lines between matches
	delete gl_matches;
	delete gl_best_votes;
	delete gl_best_pose;

}

MainRenderProcess::MainRenderProcess()
{
	
	// init the opengl window
	glm::mat4 vm = glm::lookAt(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.5), glm::vec3(0.0f, 1.0f, 0.0f));
	m_window = new isu_ar::GLViewer();
	m_window->create(1280, 1280, "TrackingExpert+ Demo");
	m_window->addRenderFcn(std::bind(&MainRenderProcess::render_fcn, this, _1, _2));
	//m_window->addKeyboardCallback(std::bind(&TrackingExpertDemo::keyboard_cb, this, _1, _2));
	m_window->setViewMatrix(vm);
	m_window->setClearColor(glm::vec4(0, 0, 0, 1));

	m_update_camera_pc = false;
}


/*
initialize the grapics system and processes.
*/
void MainRenderProcess::initGfx(void)
{
	// fetch an instance of the data manager. 
	PointCloudManager* dm = PointCloudManager::getInstance();
	if (dm == NULL) {
		std::cout << "[ERROR] -  Did not obtain DataManager pointer." << std::endl;
	}

	// camera point cloud
	gl_camera_point_cloud = new	isu_ar::GLPointCloudRenderer(dm->getCameraPC().points, dm->getCameraPC().normals); 
	gl_camera_point_cloud->setPointColor(glm::vec3(1.0, 0.0, 0.0));
	gl_camera_point_cloud->setNormalColor(glm::vec3(0.8, 0.5, 0.0));
	gl_camera_point_cloud->setNormalGfxLength(0.05f);
	gl_camera_point_cloud->enableAutoUpdate(false);
	gl_camera_point_cloud->enablePointRendering(true);

	// reference point cloud
	gl_reference_point_cloud = new	isu_ar::GLPointCloudRenderer(dm->getReferecePC().points, dm->getReferecePC().normals); 
	gl_reference_point_cloud->setPointColor(glm::vec3(0.0, 1.0, 0.0));
	gl_reference_point_cloud->setNormalColor(glm::vec3(0.0, 0.8, 0.8));
	gl_reference_point_cloud->setNormalGfxLength(0.005f);

	// point cloud for evaluation.
	gl_reference_eval = new	isu_ar::GLPointCloudRenderer(dm->getReferecePC().points, dm->getReferecePC().normals); 
	gl_reference_eval->setPointColor(glm::vec3(1.0, 1.0, 0.0));
	gl_reference_eval->setNormalColor(glm::vec3(0.5, 0.8, 0.8));
	gl_reference_eval->setNormalGfxLength(0.05f);


	//-------------------------------------------------------------------
	// Render lines

	gl_matches = new isu_ar::GLLineRenderer(dm->getReferecePC().points, dm->getCameraPC().points, match_pair_ids);  //pc_ref.points, m_pc_camera.points
	gl_matches->setLineColor(glm::vec3(1.0, 0.0, 1.0));
	gl_matches->enableRenderer(false);

	gl_best_votes = new isu_ar::GLLineRenderer(dm->getReferecePC().points, dm->getCameraPC().points, vote_pair_ids);//pc_ref.points, m_pc_camera.points
	gl_best_votes->setLineWidth(5.0);
	gl_best_votes->setLineColor(glm::vec3(1.0, 1.0, 0.0));
	gl_best_votes->enableRenderer(false);

	gl_best_pose = new isu_ar::GLLineRenderer(dm->getReferecePC().points, dm->getCameraPC().points, pose_ids); // pc_ref.points, m_pc_camera.points
	gl_best_pose->setLineWidth(5.0);
	gl_best_pose->setLineColor(glm::vec3(0.0, 1.0, 0.2));
	gl_best_votes->enableRenderer(false);

}


/*
Start the rendern loop
*/
void MainRenderProcess::start(void)
{
	// start the viewer
	m_window->start();
}


/*
To be passed to the renderer to draw the content.
*/
void MainRenderProcess::render_fcn(glm::mat4 pm, glm::mat4 vm)
{
	// update the camera data
	//updateCamera();

	if (m_update_camera_pc) {
		m_update_camera_pc = false;
		gl_camera_point_cloud->updatePoints();
	}


	// update the poses if a new scene model is available.
	//trackObject();
	renderPointCloudScene(pm, vm);
	/*switch (m_scene_type) {
		case PC:
			renderPointCloudScene(pm, vm);
			break;
		case AR:
			renderARScene(pm, vm);
			break;
	}*/
}


/*
Render the point cloud sceen and show the point cloud content
*/
void MainRenderProcess::renderPointCloudScene(glm::mat4 pm, glm::mat4 vm)
{
	// draw the camera point cloud
	gl_camera_point_cloud->draw(pm, vm);
	gl_reference_point_cloud->draw(pm, vm);
	gl_reference_eval->draw(pm, vm);

	// render lines between matches
	//gl_matches->draw(pm, vm);
	//gl_best_votes->draw(pm, vm);
	gl_best_pose->draw(pm, vm);
//
}


/*
	Update the data for the renderer
	*/
void MainRenderProcess::setUpdate(void)
{
	m_update_camera_pc = true;
}



/*
Add a keyboard function to the existing window.
@param fc -  function pointer to the keyboard function.
*/
void MainRenderProcess::setKeyboardFcn(std::function<void(int, int)> fc)
{
	m_window->addKeyboardCallback(fc);
}