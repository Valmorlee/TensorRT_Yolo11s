#include <bits/stdc++.h>
#include <string>
#include "infer.h"
#include "BYTETracker.h"


// 需要跟踪的类别，可以根据自己需求调整，筛选自己想要跟踪的对象的种类（以下对应COCO数据集类别索引）
std::vector<int>  trackClasses {0, 1, 2, 3, 5, 7};  // person, bicycle, car, motorcycle, bus, truck


bool isTrackingClass(int class_id){
	for (auto& c : trackClasses){
		if (class_id == c) return true;
	}
	return false;
}


int run(string inputVideoPath){
    // read video
    cv::VideoCapture cap(inputVideoPath);
    if ( !cap.isOpened() ) return 0;

    int img_w = cap.get(CAP_PROP_FRAME_WIDTH);
	int img_h = cap.get(CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(CAP_PROP_FPS);
    // long nFrame = static_cast<long>(cap.get(CAP_PROP_FRAME_COUNT));
    // std::cout << "Total frames: " << nFrame << std::endl;

    cv::VideoWriter writer("result1.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(img_w, img_h));

    // YOLOv8 predictor
    std::string trtFile = "../detect/build/yolo11s.plan";
    YoloDetector detector(trtFile, 0, 0.45, 0.01);

    // ByteTrack tracker
    BYTETracker tracker(fps, 30);

    cv::Mat img;
    int num_frames = 0;
    int total_ms = 0;
    while (true){
        if ( !cap.read(img) ) break;
        num_frames++; // 总帧数

        if (num_frames % 100 == 0){
            cout << "Processing frame " << num_frames << " (" << num_frames * 1000000 / total_ms << " fps)" << endl;
        }
        if (img.empty()) break;

        auto start = std::chrono::system_clock::now();

        // yolo inference
        std::vector<Detection> res = detector.inference(img);

        // yolo output format to bytetrack input format, and filter bbox by class id
        std::vector<Object> objects;
        for (size_t j = 0; j < res.size(); j++){
            float* bbox = res[j].bbox;
            float conf = res[j].conf;
            int classId = res[j].classId;

            if (isTrackingClass(classId)){
                cv::Rect_<float> rect(bbox[0], bbox[1], (bbox[2] - bbox[0]), (bbox[3] - bbox[1]));
                Object obj {rect, classId, conf};
                objects.push_back(obj);
            }
        }

        // track
        std::vector<STrack> output_stracks = tracker.update(objects);

        auto end = std::chrono::system_clock::now();
        total_ms = total_ms + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        for (int i = 0; i < output_stracks.size(); i++)
		{
			std::vector<float> tlwh = output_stracks[i].tlwh;
			// bool vertical = tlwh[2] / tlwh[3] > 1.6;
			// if (tlwh[2] * tlwh[3] > 20 && !vertical)
            if (tlwh[2] * tlwh[3] > 20)
			{
				cv::Scalar s = tracker.get_color(output_stracks[i].track_id);
				cv::putText(img, cv::format("%d", output_stracks[i].track_id), cv::Point(tlwh[0], tlwh[1] - 5), 
                        0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
                cv::rectangle(img, cv::Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3]), s, 2);
			}
		}
        cv::putText(img, cv::format("frame: %d fps: %d num: %ld", num_frames, num_frames * 1000000 / total_ms, output_stracks.size()), 
                cv::Point(0, 30), 0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        writer.write(img);

        cv::imshow("img", img);
    	char c = waitKey(1);
        if (c > 0) break;
    }

    cap.release();
    std::cout << "FPS: " << num_frames * 1000000 / total_ms << std::endl;

    return 0;
}

int run(int index){
    // read live camera 读取直播摄像头 index为摄像头号
    cv::VideoCapture cap(index,CAP_V4L2);
	cap.set(cv::CAP_PROP_FOURCC,cv::VideoWriter::fourcc('M','J','P','G'));
    if ( !cap.isOpened() ) return 0;

    int img_width = cap.get(CAP_PROP_FRAME_WIDTH);
	int img_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(CAP_PROP_FPS);
    // long long nFrame = static_cast<long>(cap.get(CAP_PROP_FRAME_COUNT));
    // cout << "Total frames: " << nFrame << endl;

	//文件录像功能
    //cv::VideoWriter writer("result.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(img_width, img_height));

    // YOLOv8 predictor
    std::string trtFile = "../detect/build/yolo11s.plan";
    YoloDetector detector(trtFile, 0, 0.45, 0.01);

    // ByteTrack tracker
    BYTETracker tracker(fps, 30);

    cv::Mat img;
    int num_frames = 0;
    int total_ms = 0;
    while (true){
        if ( !cap.read(img) ) break;
    	long long FPS = num_frames * 1000000 / total_ms;

        num_frames++;
        if (num_frames % 100 == 0){ //区间测速
            cout << "正在处理第 " << num_frames << " 张" << " ( 区间均帧: " << FPS << " fps)" << endl;
        }
        if (img.empty()) break;

        auto start = std::chrono::system_clock::now();

        // yolo inference
        std::vector<Detection> res = detector.inference(img);

        // yolo output format to bytetrack input format, and filter bbox by class id
        std::vector<Object> objects;
        for (size_t j = 0; j < res.size(); j++){
            float* bbox = res[j].bbox;
            float conf = res[j].conf;
            int classId = res[j].classId;

            if (isTrackingClass(classId)){
                cv::Rect_<float> rect(bbox[0], bbox[1], (bbox[2] - bbox[0]), (bbox[3] - bbox[1]));
                Object obj {rect, classId, conf};
                objects.push_back(obj);
            }
        }

        // track
        std::vector<STrack> output_stracks = tracker.update(objects);

        auto end = std::chrono::system_clock::now();
        total_ms = total_ms + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        for (int i = 0; i < output_stracks.size(); i++)
		{
			std::vector<float> tlwh = output_stracks[i].tlwh;
			// bool vertical = tlwh[2] / tlwh[3] > 1.6;
			// if (tlwh[2] * tlwh[3] > 20 && !vertical)
            if (tlwh[2] * tlwh[3] > 20)
			{
				cv::Scalar s = tracker.get_color(output_stracks[i].track_id);
				cv::putText(img, cv::format("ID: %d", output_stracks[i].track_id), cv::Point(tlwh[0], tlwh[1] - 5),
                        0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
                cv::rectangle(img, cv::Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3]), s, 2);
			}
		}

    	long long FPS2 = num_frames * 1000000 / total_ms;
        cv::putText(img, cv::format("FPS: %lld num: %ld", FPS2, output_stracks.size()),
                cv::Point(0, 30), 0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

        //writer.write(img); //index启用录像功能

		resize(img,img,Size(img_width, img_height));
        cv::imshow("img", img);
    	char c = waitKey(1);
        if (c > 0) break;
    }

    cap.release();
    std::cout << "FPS: " << num_frames * 1000000 / total_ms << std::endl;

    return 0;
}


// int main(int argc, char* argv[]){
//     if (argc != 2 )
//     {
//         std::cerr << "arguments not right!" << std::endl;
//         std::cerr << "Usage: ./main [video path]" << std::endl;
//         std::cerr << "Example: ./main ./videos/demo.mp4" << std::endl;
//         return -1;
//     }
//
//     return run(argv[1]);
// }

int main(){

	string videoPath = "/home/valmorx/CLionProjects/OpenCV_Yolov11/Lib_Photo/01.mp4";

	return run(0);
}