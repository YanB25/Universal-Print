#include "glog/logging.h"

DEFINE_string(msg, "helloworld", "The message to print");

int main(int argc, char* argv[])
{
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_logtostderr = true;
    FLAGS_colorlogtostderr = true;
    
    LOG(INFO) << FLAGS_msg;
    
    return 0;
}