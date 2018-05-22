/******************
  by cjf@xmu.2018/02/03
*****************/
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

#define PI 3.14159
#define INF 1e10
struct POINT{
  int x,y;
  POINT(int x=0,int y=0):x(x),y(y){}
};
struct RECT{
  POINT pnt[4];                               //矩形
  RECT(){}
  RECT(pair<float,float> p1,pair<float,float> p2,pair<float,float> p3,pair<float,float>p4){
    pnt[0].x=(int)p1.first;  pnt[0].y=(int) p1.second;
    pnt[1].x=(int)p2.first;  pnt[1].y=(int) p2.second;
    pnt[2].x=(int)p3.first;  pnt[2].y=(int) p3.second;
    pnt[3].x=(int)p4.first;  pnt[3].y=(int) p4.second;
  }
  cv::Point get(int idx){
	  return cv::Point(pnt[idx].y,pnt[idx].x);
	  //return cv::Point(pnt[idx].x,pnt[idx].y);
	}
};
class DETECTOR{
private:
	//参数
#define  RESERVED        0.25
#define  BLOCK_MIN_SIZE  0.0005                //小块的最小大小
	//attribute
	cv::Mat m_img;                               //原始图片
	cv::Mat m_mask;                              //二值mask
	int     m_rc_hist[256];                      //red channels直方图
  int     m_rc_hist_max;                       //直方图最大值
	int     m_keypoint1;                         //红色通道后RESERVED处像素值
	int     m_rough_mask_cnt;
	float   m_plaquerate;                          //牙菌斑比例
	vector<RECT>  m_rect;
	bool ** m_vis;
	inline bool   validPos(int r,int c);
	//method
    int    bfs(int r,int c,vector<pair<int,int> > &pnts);
	//m_img像素读取。读取b,g,r像素 b,g,r->0,1,2
	inline uchar     at(int r,int c,int channel);
	inline cv::Vec3b at(int r,int c);
  //inline int       set(int r,int c,cv::Vec3b pixel);
  //m_mask像素读取和设置
	inline uchar get_mask(int r,int c);
	inline int   set_mask(int r,int c,uchar val);
	int    init(cv::Mat& img);                      //初始化member
	pair<float,float> lineIntersection(float a,float b,float p,float q); //直线求交点
  bool   inMouth();                            //m_img是否是口腔照片
	int    removeLittleBlock();                  //删除小块
    int    getRoughMask();                       //得到粗糙的区域
    int    pruneMask();                          //得到较精确的区域
    int    drawRects(cv::Mat& contour);          //将方框M_rect画在contour里面
	int    drawContour(cv::Mat& contour);
    int    getRectContainPnts(vector<pair<int,int> >& pnts); //选择一个最小的矩形，包含所有pnts中的点
public:
    DETECTOR(){}
	~DETECTOR(){}
	int  processImage(cv::Mat& img,cv::Mat& mask,cv::Mat& contour);   //成功返回
	void save(cv::Mat& img,char* filename);
	int  getPlaqueInfo(vector<RECT>* rects,float* rate);              //返回牙菌斑信息：包围的矩形，以及牙菌斑比例。

};

int  DETECTOR::getPlaqueInfo(vector<RECT>*rects,float* rate){
  (*rate)=m_plaquerate;
	for (int i=0;i<m_rect.size();i++)
		rects->push_back(m_rect[i]);
	return 0;
}
bool DETECTOR::inMouth(){
  float r_mean=0;
	float g_mean=0;
	float b_mean=0;
	int nWhite=0;  //白色像素统计
	int nRed=0;    //红色像素统计
	int nBlack=0;
	for (int r=0;r<m_img.rows;r++)
		for (int c=0;c<m_img.cols;c++){
		  cv::Vec3b p3=at(r,c);
      int b=p3[0];
			int g=p3[1];
			int r=p3[2];
			int sum2=b+g+r-min(min(g,b),r);
			if (sum2>100*2) nWhite++;
			if (r>100 && 2*r>b+g+60) nRed++;
			if (b+g+r-max(max(b,g),r) <20*2) nBlack++;
		}
	float rateWhite=1.0*nWhite/m_img.rows/m_img.cols;
	float rateRed=1.0*nRed/m_img.rows/m_img.cols;
	float rateBlack=1.0*nBlack/m_img.rows/m_img.cols;
	if ( rateBlack<0.7&&rateWhite<0.7&&rateWhite>0.15 && rateRed<0.45 && rateRed>0.05)
	return true;
	else
	return false;
}
pair<float,float> DETECTOR::lineIntersection(float a,float b,float p,float q){ //
   //y=ax+b与y=px+q求交点。(不判断平行的情况)
   float x=(q-b)/(a-p);
   float y=a*x+b;
   return make_pair(x,y);
}
inline bool DETECTOR::validPos(int r,int c){
  return r>=0 && c>=0 && r<m_img.rows&&c<m_img.cols;
}
int DETECTOR::drawContour(cv::Mat& contour){
  contour=m_img.clone();
	for (int r=0;r<m_mask.rows;r++)
		for (int c=0;c<m_mask.cols;c++)
		if (get_mask(r,c)){
			bool belong=false;
			if (validPos(r-1,c) && get_mask(r-1,c)==0) belong=true;
			if (validPos(r+1,c) && get_mask(r+1,c)==0) belong=true;
			if (validPos(r,c-1) && get_mask(r,c-1)==0) belong=true;
			if (validPos(r,c+1) && get_mask(r,c+1)==0) belong=true;
			if(belong){
				contour.at<cv::Vec3b>(r,c)[0]=0xff;
			  contour.at<cv::Vec3b>(r,c)[1]=0xff;
				contour.at<cv::Vec3b>(r,c)[2]=0xff;
			}
		}
	return 0;
}
int DETECTOR::drawRects(cv::Mat& contour){
  contour=m_img.clone();
	for (int i=0;i<m_rect.size();i++){
	  RECT rect=m_rect[i];
		cv::Scalar color(0x55,0xff,0x55);
		int thickness=max(2,m_img.rows>>7);
		for (int p=0;p<4;p++){
			cv::Point p1=rect.get(p);
			cv::Point p2=rect.get((p+1)%4);
			//printf("drawLine (%d,%d)->(%d,%d)\n",p1.x,p1.y,p2.x,p2.y);
			cv::line(contour,p1,p2,color,thickness);
		}
	}
	return 0;
}
inline int DETECTOR::set_mask(int r,int c,uchar val){
  m_mask.ptr<uchar>(r)[c]=val;
	return 0;
}
inline cv::Vec3b DETECTOR::at(int r,int c){
  return m_img.ptr<cv::Vec3b>(r)[c];
}
int DETECTOR::getRoughMask(){
  int pixels_res = int(RESERVED*m_img.rows*m_img.cols);
  int cur_pixel=256;
	//保留部分较亮像素作为初始区域
	while (pixels_res>0){
		cur_pixel--;
	  pixels_res-=m_rc_hist[cur_pixel];
	}
	m_keypoint1=cur_pixel;/*
	int hist[256];
  for (int i=0;i<256;i++){
	  hist[i]=m_rc_hist[i];
		printf("%d ",m_rc_hist[i]);
	}
	sort(hist,hist+256);*/
	int mx_cnt=m_rc_hist_max;
	int key_pixels=0;
	double valid_sum=0;
	int valid_cnt=0;
	for (int i=190;i>=50;i--){
	  double cur_val=0;
		for (int j=0;j<5;j++) cur_val+=1.0*m_rc_hist[i-j]/mx_cnt;
		cur_val/=5;
		if (cur_val>0.4){
		  key_pixels=i+5;
			break;
		}
		if (cur_val>0.005){
		  if (valid_cnt>0 && valid_sum/valid_cnt<cur_val-0.15){
			  key_pixels=i+5;
				break;
			}else{
			  valid_sum+=cur_val;
			  valid_cnt++;
			}
		}
	}
	m_rough_mask_cnt=0;
	for (int r=0;r<m_img.rows;r++)
		for (int c=0;c<m_img.cols;c++){
		   cv::Vec3b p3=at(r,c);
			 int red=p3[2];
			 int green=p3[1];
			 int blue=p3[0];
			 int dist=(red-green)*(red-green)+(red-blue)*(red-blue);
			 if (red > cur_pixel && red>=key_pixels && red>green && red> blue && dist>2*30*30){
				 set_mask(r,c,0xff);
			   m_rough_mask_cnt++;
			 }
			 else
				 set_mask(r,c,0x00);
		}
	return 0;
}
int DETECTOR::bfs(int r,int c,vector<pair<int,int> > &pnts){
    pnts.clear();
	pnts.push_back(make_pair(r,c));
	m_vis[r][c]=true;
	int l=0;
	int color=get_mask(r,c);
	while (l<pnts.size()){
	  pair<int,int> pnt=pnts[l++];
		int x=pnt.first;
		int y=pnt.second;
		//up
		if (x>0 && !m_vis[x-1][y] && get_mask(x-1,y)==color){
		  pnts.push_back(make_pair(x-1,y));
			m_vis[x-1][y]=true;
		}
		//down
		if (x<m_mask.rows-1 && !m_vis[x+1][y] && get_mask(x+1,y)==color){
		  pnts.push_back(make_pair(x+1,y));
		  m_vis[x+1][y]=true;
		}
		//left
		if (y>0 && !m_vis[x][y-1] && get_mask(x,y-1)==color){
		  pnts.push_back(make_pair(x,y-1));
			m_vis[x][y-1]=true;
		}
		//right
		if (y<m_mask.cols-1 && !m_vis[x][y+1] && get_mask(x,y+1)==color){
		  pnts.push_back(make_pair(x,y+1));
			m_vis[x][y+1]=true;
		}
	}
}
int DETECTOR::removeLittleBlock(){
  //vis空间申请
	m_vis=(bool**)malloc(sizeof(int*)*m_mask.rows);
	for(int r=0;r<m_mask.rows;r++){
		m_vis[r]=(bool*)malloc(sizeof(bool)*m_mask.cols);
	  memset(m_vis[r],0,sizeof(bool)*m_mask.cols);
	}
	//遍历
  vector<pair<int,int> > pnts;
	for(int r=0;r<m_mask.rows;r++)
		for (int c=0;c<m_mask.cols;c++)
		if (!m_vis[r][c]){
	    pnts.clear();
			bfs(r,c,pnts);
				uchar color=(get_mask(r,c)==0?0xff:0);
			if (pnts.size()<m_mask.rows*m_mask.cols*BLOCK_MIN_SIZE){
			  //去除小块
				for (int i=0;i<pnts.size();i++){
				  pair<int,int> pnt=pnts[i];
					set_mask(pnt.first,pnt.second,color);
				}
			}else{
				//大块区域且颜色为黑，说明是牙菌斑。
			  if (color==0x00){
					getRectContainPnts(pnts);
				  m_plaquerate+=1.0*pnts.size()/m_mask.rows/m_mask.cols;
				}
			}
	}
	//vis空间释放
	for (int r=0;r<m_mask.rows;r++)
		free(m_vis[r]);
	free(m_vis);
}
inline uchar  DETECTOR::at(int r,int c,int channel){
	return m_img.ptr<cv::Vec3b>(r)[c][channel];
}
inline uchar  DETECTOR::get_mask(int r,int c){
  return m_mask.ptr<uchar>(r)[c];
}
int DETECTOR::init(cv::Mat& img){
    m_rect.clear();
		m_plaquerate=0.0;
    m_img  = img.clone();
	m_mask.create(m_img.rows,m_img.cols,CV_8UC1);
	//m_rc_hist
	memset(m_rc_hist,0,sizeof(m_rc_hist));
	for (int r=0;r<m_img.rows;r++)
		for (int c=0;c<m_img.cols;c++)
			m_rc_hist[at(r,c,2)]++;
	m_rc_hist_max=0;
	for (int i=0;i<256;i++)
		m_rc_hist_max=max(m_rc_hist_max,m_rc_hist[i]);
	return 0;
}
int DETECTOR::pruneMask(){
  //按照当前前景像素占的比例，取不同位置为基准点。往前扩展一定的像素值。
	float share = 1.0 * m_rough_mask_cnt/m_mask.rows/m_mask.cols;
  int   threshold;
	if (share>0.2) threshold=20;
	else
		if (share>0.12) threshold=30;
	  else
			if (share>0.08) threshold=40;
	    else
		   threshold=60;
  //取20%(即总像素的RESERVED*0.2)处的像素值大小
	int cur_pixel=256;
	int sum=(int) m_rough_mask_cnt*0.2;
	//printf("占比%lf threshold:%d 前景像素个数 %d sum=%d\n",share,threshold,m_rough_mask_cnt,sum);
	while (sum>0){
	  cur_pixel--;
		sum-=m_rc_hist[cur_pixel];
	}
	//printf("keypoint[2] %d\n",cur_pixel);

	//清除与基准色相差较远的像素
	for (int r=0;r<m_mask.rows;r++)
		for (int c=0;c<m_mask.cols;c++)
			if (get_mask(r,c)!=0 && cur_pixel-at(r,c,2)>threshold)
				set_mask(r,c,0);
}
int DETECTOR::processImage(cv::Mat& img,cv::Mat& ou_Mask,cv::Mat& ou_Contour){
	init(img);
	if (!inMouth()) return false;
	getRoughMask();
  //pruneMask();
	removeLittleBlock();
	ou_Mask=m_mask.clone();
	drawRects(ou_Contour);
	return true;
}
void DETECTOR::save(cv::Mat& img,char* filename){
  //cv::imwrite(filename,img);
	//printf("图片保存到:%s\n",filename);
}
int DETECTOR::getRectContainPnts(vector<pair<int,int> >& pnts){//面积最小的矩形包含所有点
  //矩形下边旋转degree度后的形成新矩形，用于覆盖pnts。选择面积最小的一个
  float bestDegree=0;
  float bestArea=INF;
  pair<float,float>bestP1;
  pair<float,float>bestP2;
  pair<float,float>bestP3;
  pair<float,float>bestP4;
  //
  for (float degree=5;degree<90;degree+=5){  //从5度开始，避免特殊判断直线x=10之类的情况。
    //4条相互平行、垂直的执行包围所有点
    //(1)直线:y=ax+b1 与y=ax+b2;

    float a=tan(degree/180*PI);
    float b1=INF,b2=-INF;
    for (int i=0;i<pnts.size();i++){
        int x=pnts[i].first;
        int y=pnts[i].second;
        float b=y-a*x;
        b1=min(b,b1);
        b2=max(b,b2);
    }
    //(2)直线：y=px+q1,y=px+q2 与上面两条平行(或者x=q1,x=q2);
    float p=tan((degree+90)/180*PI);
    float q1=INF,q2=-INF;
    for (int i=0;i<pnts.size();i++){
        int x=pnts[i].first;
        int y=pnts[i].second;
        float q=y-p*x;
        q1=min(q1,q);
        q2=max(q2,q);
    }
  //当前面积
  float area=(b2-b1)*cos(degree/180*PI) * (q2-q1)*sin((degree)/180*PI);
  if (bestArea>area){ //找到更优的
    bestArea=area;
    bestDegree=degree;
    bestP1=lineIntersection(a,b1,p,q2);
    bestP2=lineIntersection(a,b2,p,q2);
    bestP3=lineIntersection(a,b2,p,q1);
    bestP4=lineIntersection(a,b1,p,q1);
  }
  }
  m_rect.push_back(RECT(bestP1,bestP2,bestP3,bestP4));
  return 0;
}

//测试
cv::Mat mask;
cv::Mat contour;
cv::Mat img;    //
DETECTOR mgr;   //可以处理多张图片(后面执行的图片会覆盖前面的数据)    
void test(char* filename){
  //读取图片
	img=cv::imread(filename);
	int rst=mgr.processImage(img,mask,contour);//img是输入值,mask是输出的二值图,contour是在原图上画个轮廓。三个参数都需要预先定义。
	if (rst==1){//是口腔图片
		printf("[*]是口腔图片\n");
	  //
		float rate; //牙菌斑比例
		vector<RECT> rects;
		//返回牙菌斑的矩形和占的比例
		mgr.getPlaqueInfo(&rects,&rate);
		for (int i=0;i<rects.size();i++){
		   printf("矩形[%d],四个像素点位置:",i);
			 for (int j=0;j<4;j++) printf("(%d,%d)  ",rects[i].pnt[j].x,rects[i].pnt[j].y);
		   putchar('\n');
		}
		printf("牙菌斑比例:%.3f\n",rate);
		//保存
		cv::imwrite("msk.jpg",mask);
	  cv::imwrite("contour.jpg",contour);
	}else{//不是口腔图片
	  printf("[!]不是口腔图片\n");
	}

}
int main(int argc,char** argv){
	char *filename=argv[1];
	test(filename);
	return 0;
}
