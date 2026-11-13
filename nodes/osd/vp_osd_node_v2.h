#pragma once

#include "../vp_node.h"

namespace vp_nodes {
    // on screen display(short as osd) node.
    // another version for vp_frame_target display, display vp_sub_target at the bottom of screen.
    class vp_osd_node_v2: public vp_node
    {
    private:
        // leave a gap at the bottom of osd frame
        int gap_height = 256;
        int padding = 10;
    protected:
        virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) override;
    public:
        vp_osd_node_v2(std::string node_name);
        ~vp_osd_node_v2();
    };

}