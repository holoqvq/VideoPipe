
#pragma once

#include "../vp_primary_infer_node.h"

namespace vp_nodes {
    // gauge detector based on yolo, see vp_yolo_detector_node also.
    // output 4 bbox + 1 class score for single class gauge.
    class vp_yolo_gauge_detector_node final: public vp_primary_infer_node
    {
    private:
        float score_threshold;
        float nms_threshold;
        std::string gauge_label;

        int to_frame_x(float x, bool normalized, int frame_width);
        int to_frame_y(float y, bool normalized, int frame_height);
    protected:
        virtual void postprocess(const std::vector<cv::Mat>& raw_outputs, const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) override;
    public:
        vp_yolo_gauge_detector_node(std::string node_name, 
                            std::string model_path, 
                            int input_width = 640, 
                            int input_height = 640,
                            int class_id_offset = 0,
                            float score_threshold = 0.25,
                            float nms_threshold = 0.45,
                            float scale = 1 / 255.0,
                            cv::Scalar mean = cv::Scalar(0),
                            cv::Scalar std = cv::Scalar(1),
                            bool swap_rb = true,
                            std::string gauge_label = "gauge");
        ~vp_yolo_gauge_detector_node();
    };
}
