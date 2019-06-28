//=============================================================================
// OpenCV를 사용하여 동영상 녹화하기
//
// API설명 참조 사이트 : https://opencvlib.weebly.com
//=============================================================================

#include <opencv/cv.h>
#include <opencv/highgui.h>
 
int main() {
    IplImage *frame = 0;
    CvVideoWriter *writer = 0;
 
 	//==========================================
 	// 폰트 설정
 	//==========================================
    CvFont font;
    //cvInitFont(폰트, 폰트이름, 글자 수평스케일, 글자 수직스케일, 기울임, 굵기) : 폰트 초기화
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 5);
    
    //==========================================
 	// 카메라 장치로부터 영상을 획득할 구조체 포인터 받아오기
 	//==========================================
    //cvCaptureFromCAM(int index)
    //Camera로부터 영상을 받아 온다. CvCapture 형태의 포인터로 저장한다.
	CvCapture* capture = cvCaptureFromCAM(0);

    //==========================================
 	// 동영상 파일로부터 영상을 읽어올 구조체 포인터 받아오기
 	//==========================================
    //cvCaptureFromFile(파일명) : 동영상 파일 불러오기
    //CvCapture *capture = cvCaptureFromAVI("D:/study/su.avi");
 
    //==========================================
 	// "video"라는 이름의 윈도우 생성
 	//==========================================
    cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
    
    int fps = 30;    //30프레임
 
    //==========================================
 	// 영상 1-Frame 캡쳐
 	// 영상의 정보를 획득하기 위해 
 	//==========================================
	//카메라나 동영상 파일에서 프레임을 잡아낸다.
	//프레임은 압축 형식으로 저장되기 때문에 직접 나타나진 않는다. 
	//이 프레임을 읽고 싶다면 cvRetrieveFrame() 함수를 사용한다.
    cvGrabFrame(capture);
    frame = cvRetrieveFrame(capture);
    
    //==========================================
 	// 영상을 저장할 비디오 파일 생성
 	//==========================================    
    //cvCreateVideoWriter(경로, 코덱방식, fps, 프레임크기) : 동영상파일 생성
    //CV_FOURCC('D', 'I', 'V', 'X') : MPEG-4 코덱
    writer = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvGetSize(frame));

    while (1) {
        cvGrabFrame(capture);
        frame = cvRetrieveFrame(capture);
 
     	//==========================================
 		// 영상 파일이 다 출력했거나 키보드 입력이 들어왔다면
 		// while문을 빠져나온다.
 		//========================================== 
        if (!frame || cvWaitKey(10) >= 0) { break; }
 
        //cvPutText(이미지, 텍스트, 출력위치, 폰트, 텍스트색상) : 텍스트를 이미지에 추가
        cvPutText(frame, "TEST TEXT", cvPoint(100, 100), &font, CV_RGB(255,0,0));

        //cvWriteFrame(동영상, 프레임) : 지정한 동영상에 프레임을 쓴다
        cvWriteFrame(writer, frame);
 
        cvShowImage("video", frame);
    }
 
    //cvReleaseVideoWriter(동영상) : 메모리에서 해제한다
    cvReleaseVideoWriter(&writer);
 
    cvReleaseCapture(&capture);
    cvDestroyWindow("video");
    return 0;
}

