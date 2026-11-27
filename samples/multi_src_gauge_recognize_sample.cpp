#include "../nodes/vp_app_src_node.h"
#include "../nodes/infers/vp_yolo_gauge_detector_node.h"
#include "../nodes/infers/vp_resnet_gauge_regressor_node.h"
#include "../nodes/osd/vp_gauge_value_osd_node.h"
#include "../nodes/vp_split_node.h"
#include "../nodes/vp_screen_des_node.h"

#include "../utils/analysis_board/vp_analysis_board.h"

/*
* ## multi src gauge recognize sample ##
* copy of multi_src_face_recognize_sample: 3 different inputs (image, video, camera) merge into 1 branch
* for 1 gauge recognize task, then resume to 3 branches for outputs again.
* 1. image input: read by cv::imread(...) in host code, based on vp_app_src_node
* 2. video input: read by cv::VideoCapture(...) in host code, based on vp_app_src_node
* 3. camera input: captured by cv::VideoCapture(...) in host code, based on vp_app_src_node
* push frames for each input in order, camera keeps pushing after image and video are finished.
* detect gauges(yolo_gauge_detector), regress gauge reading(resnet_gauge_regressor), then display on screen with osd for each channel.
*/

int main() {
    VP_SET_LOG_INCLUDE_THREAD_ID(false);
    VP_SET_LOG_LEVEL(vp_utils::vp_log_level::INFO);
    VP_LOGGER_INIT();

    // create nodes
    auto app_src_0 = std::make_shared<vp_nodes::vp_app_src_node>("app_src_0", 0);  // for image
    auto app_src_1 = std::make_shared<vp_nodes::vp_app_src_node>("app_src_1", 1);  // for video
    auto app_src_2 = std::make_shared<vp_nodes::vp_app_src_node>("app_src_2", 2);  // for camera

    // model 1: gauge detector (yolo)
    auto yolo_gauge_detector = std::make_shared<vp_nodes::vp_yolo_gauge_detector_node>(
        "yolo_gauge_detector_0", "./models/det_yolo26n.onnx", 640, 640, 0, 0.25f, 0.45f);

    // model 2: gauge regressor (resnet), applied to class id 0 (the detector uses class_id_offset 0)
    auto resnet_gauge_regressor = std::make_shared<vp_nodes::vp_resnet_gauge_regressor_node>(
        "resnet_gauge_regressor_0", "./models/reg_resnet50.onnx", 256, 256, std::vector<int>{0}, 0, 0, 0.088f);

    auto split = std::make_shared<vp_nodes::vp_split_node>("split", true);  // split by channel index

    auto osd_0 = std::make_shared<vp_nodes::vp_gauge_value_osd_node>("osd_0");
    auto screen_des_0 = std::make_shared<vp_nodes::vp_screen_des_node>("screen_des_0", 0);

    auto osd_1 = std::make_shared<vp_nodes::vp_gauge_value_osd_node>("osd_1");
    auto screen_des_1 = std::make_shared<vp_nodes::vp_screen_des_node>("screen_des_1", 1);

    auto osd_2 = std::make_shared<vp_nodes::vp_gauge_value_osd_node>("osd_2");
    auto screen_des_2 = std::make_shared<vp_nodes::vp_screen_des_node>("screen_des_2", 2);

    // construct pipeline
    yolo_gauge_detector->attach_to({app_src_0, app_src_1, app_src_2});
    resnet_gauge_regressor->attach_to({yolo_gauge_detector});

    split->attach_to({resnet_gauge_regressor});

    // split by vp_split_node, make sure the order is the same as channel index
    osd_0->attach_to({split});
    osd_1->attach_to({split});
    osd_2->attach_to({split});

    // auto split again on channel 0
    screen_des_0->attach_to({osd_0});

    // auto split again on channel 1
    screen_des_1->attach_to({osd_1});

    // auto split again on channel 2
    screen_des_2->attach_to({osd_2});

    // start pipeline
    app_src_0->start();
    app_src_1->start();
    app_src_2->start();

    // for debug purpose
    vp_utils::vp_analysis_board board({app_src_0, app_src_1, app_src_2});
    board.display(1, false);  // no block since we need interactions from console later

    // data sources: one image + one video from `uploads`, change them as needed.
    const std::string image_path = "./uploads/images/000000.jpg";
    const std::string video_path = "./uploads/videos/gauge1.mp4";

    // read image and open video/camera in host code manually
    auto img = cv::imread(image_path);
    assert(!img.empty());
    cv::VideoCapture file_capture(video_path);
    cv::VideoCapture camera_capture(0);  // camera index, change it according to your device
    if (!camera_capture.isOpened()) {
        VP_WARN("open camera failed!");
    }

    // only push 1 frame every `infer_frame_step` frames, so NOT all frames enter the infer nodes (1 means no skip).
    // tune it according to inference speed and expected load.
    const int infer_frame_step = 5;

    // push frames for each input in order in a separate thread, camera keeps pushing after video finished
    bool exit = false;
    auto push_run = [&]() {
        bool image_done = false;
        bool video_done = false;
        auto video_fps = file_capture.get(cv::CAP_PROP_FPS);
        auto delta = std::chrono::milliseconds(static_cast<int>(1000 / (video_fps > 0 ? video_fps : 25)));
        int video_index = 0;
        int camera_index = 0;
        while (!exit) {
            // 1. push image to pipeline, only 1 frame
            if (!image_done) {
                app_src_0->push_frames({img});
                image_done = true;
            }

            // 2. push 1 frame from video to pipeline, but skip some frames
            if (!video_done) {
                cv::Mat video_frame;
                if (file_capture.read(video_frame) && !video_frame.empty()) {
                    if (video_index % infer_frame_step == 0) {
                        app_src_1->push_frames({video_frame});
                    }
                    video_index++;
                }
                else {
                    std::cout << "video input finished, camera continues..." << std::endl;
                    video_done = true;
                }
            }

            // 3. push 1 frame from camera to pipeline, but skip some frames
            if (camera_capture.isOpened()) {
                cv::Mat camera_frame;
                if (camera_capture.read(camera_frame) && !camera_frame.empty()) {
                    if (camera_index % infer_frame_step == 0) {
                        app_src_2->push_frames({camera_frame});
                    }
                    camera_index++;
                }
            }
            else if (video_done) {
                // no more frames can be pushed from any input
                break;
            }

            // sleep for a while to keep the same rate as video
            if (!video_done) {
                std::this_thread::sleep_for(delta);
            }
        }
    };
    std::thread push_thread(push_run);

    // pipeline will process the pushed frames asynchronously and display results here...
    std::string wait;
    std::getline(std::cin, wait);

    exit = true;
    push_thread.join();
    app_src_0->detach_recursively();
}
