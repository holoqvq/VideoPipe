
#pragma once
#include <opencv2/imgproc.hpp>

#include "../vp_node.h"

namespace vp_nodes {
    // on screen display(short as osd) node.
    // mainly used to display vp_frame_text_target on frame.
    class vp_text_osd_node: public vp_node
    {
    protected:
        virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) override;
    public:
        vp_text_osd_node(std::string node_name);
        ~vp_text_osd_node();
    };

}