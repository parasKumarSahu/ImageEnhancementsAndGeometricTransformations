#include <opencv2/opencv.hpp>
#include <string>
#include <stdint.h>
#include <math.h>
#define PI 3.14159265
using namespace std;
using namespace cv;

double interpolation(Mat &image, double x, double y, int r, int c, int option){
	int x1, y1, x2, y2, x3, y3, x4, y4, A, B, C, D;
	x1=(int)x; y1=(int)y;
	x=x-x1; y=y-y1;
	x2=x1; y2=y1+1;
	x3=x1+1; y3=y1+1;
	x4=x1+1; y4=y1;
	A=image.at<uint8_t>(x1,y1);
	B=image.at<uint8_t>(x2,y2);
	C=image.at<uint8_t>(x3,y3);
	D=image.at<uint8_t>(x4,y4);

	int gray;
	if(r>=image.rows-2 && c>=image.cols-2){
		gray=A;
	}
	else if(r>=image.rows-2){
		gray=(A*(1-y)+B*y);
	}
	else if(c>=image.cols-2){
		gray=(A*(1-x)+D*x);
	}
	else{
		gray = (int)(A*(1-x)*(1-y)+B*(1-x)*y+C*x*y+D*x*(1-y))&255;
	}
	if(option==1){
		gray=A;
	}
	return gray;
}

Mat translation(Mat image, float x, float y){
	Mat image2;
	float X, Y;
	x=image.rows*x/100;
	y=image.cols*y/100;
	X=abs(x)+image.rows;
	Y=abs(y)+image.cols;
	image2 = Mat::ones(X, Y, CV_8UC1);

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			if(x>=0&&y>=0){
				if(r>X-x||c>Y-y){
					image2.at<uint8_t>(r, c) = 0;
				}
				else{
					 image2.at<uint8_t>(r, c) = image.at<uint8_t>(r, c);
				}
			}
			else if(x<0&&y<0){
				if(r+x<0||c+y<0){
					image2.at<uint8_t>(r, c) = 0;
				}
				else{
					 image2.at<uint8_t>(r, c) = image.at<uint8_t>(r+x, c+y);
				}
			}
			else if(x<0&&y>=0){
				if(r+x<0||c>Y-y){
					image2.at<uint8_t>(r, c) = 0;
				}
				else{
					 image2.at<uint8_t>(r, c) = image.at<uint8_t>(r+x, c);
				}
			}
			else{
				if(r>X-x||c+y<0){
					image2.at<uint8_t>(r, c) = 0;
				}
				else{
					 image2.at<uint8_t>(r, c) = image.at<uint8_t>(r, c+y);
				}
			}
		}
	}
	return image2;
}

Mat image_resize(Mat image, double mX, double mY, int option){
	string str="/home/paras/ME5.jpg";
	Mat image2;
	image2 = Mat::zeros(image.rows*mX, image.cols*mY, CV_8UC1);
	double x_ratio, y_ratio;
	x_ratio=1/mX;
	y_ratio=1/mY;

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			double x, y;
			x=(r+.5)*x_ratio; y=(c+.5)*y_ratio;
			image2.at<uint8_t>(r,c)=interpolation(image,x, y, r, c, option);
		}
	}
	return image2;
}

Mat rotation(Mat image, double angle, int option){
	/*

	561.577->128 -29.8092->128
	348.804->257 317.127->514
	710.873->514 348.804->257
	[0.6427876104504119, -0.7660444424780128, 0;
	 0.7660444424780128, 0.6427876104504119, 0;
	 0, 0, 1]
	[0.6427876104504119, 0.7660444424780128, -0;
	 -0.7660444424780128, 0.6427876104504119, 0;
	 0, -0, 1]


	*/
	string str="/home/paras/ME6.png";
	Mat image2, image3, rot, xy, tr, tr2;
	angle=-50;
	double shift=0;
	float tx=1, ty=1;
	if(image.rows>image.cols){
		ty=(float)image.rows/image.cols;
	}
	else{
		tx=(float)image.cols/image.rows;
	}
	image3 = translation(image, 50*tx, 50*ty);
	if(image3.rows>image3.cols){
		ty=(float)image3.rows/image3.cols;
	}
	else{
		tx=(float)image3.cols/image3.rows;
	}
	image2 = translation(image3, -34*tx, -34*ty);
	image = translation(image3, -34*tx, -34*ty);

	rot = Mat::zeros(3, 3, CV_64F);
	tr = Mat::zeros(3, 3, CV_64F);
	tr2 = Mat::zeros(3, 3, CV_64F);
	shift=max(image.cols, image.rows)/2;
	xy = Mat::zeros(3, 1, CV_64F);

	xy.at<double>(2,0)=1;

	rot.at<double>(0,0)=cos((angle*PI)/180);
	rot.at<double>(1,0)=-1*sin((angle*PI)/180);
	rot.at<double>(1,1)=cos((angle*PI)/180);
	rot.at<double>(0,1)=sin((angle*PI)/180);
	rot.at<double>(2,2)=1;

	tr.at<double>(0,2)=-1*shift;
	tr.at<double>(1,2)=-1*shift;
	tr.at<double>(0,0)=1;
	tr.at<double>(1,1)=1;
	tr.at<double>(2,2)=1;

	tr2.at<double>(0,2)=shift;
	tr2.at<double>(1,2)=shift;
	tr2.at<double>(0,0)=1;
	tr2.at<double>(1,1)=1;
	tr2.at<double>(2,2)=1;

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
				xy.at<double>(0,0)=r;
				xy.at<double>(1,0)=c;
				double x, y;
				xy=tr2*rot*tr*xy;
				x=xy.at<double>(0,0);
				y=xy.at<double>(1,0);
/*				if((r==image.rows/2&&c==image.cols/4)
						||(r==image.rows/4&&c==image.cols/2)
						   ||(r==image.rows/8&&c==image.cols/8)){
					cout<<x<<"->"<<r<<" "<<y<<"->"<<c<<endl;
				}
*/
					if(x>=image.rows||y>=image.cols||x<=0||y<=0){
						image2.at<int8_t>(r, c)=0;
					}
					else{
						//image2.at<int8_t>(r, c) = image.at<int8_t>(x, y);
						image2.at<int8_t>(r, c) = interpolation(image, x, y, r, c, option);
					}
		}
	}
//	cout<<rot<<endl<<rot.inv();
	return image2;
}

Mat shearing(Mat image3, double x, double y, int option){
	/*

	216.8->140 64->64
	588.2->281 256->256
	716.6->563 128->128
	[1, 1.2, 0;
	 0, 1, 0;
	 0, 0, 1]
	[1, -1.2, 0;
	 0, 1, 0;
	 0, 0, 1]
*/
	string str="/home/paras/ME6.png";
	Mat image2, image, shear, xy;
	double X, Y;
	X=x*image3.cols/image3.rows;
	Y=y*image3.rows/image3.cols;
	image2 = translation(image3, -1*X, -1*Y);
	image = translation(image3, -1*X, -1*Y);
	x=x/100;
	y=y/100;
	shear = Mat::zeros(3, 3, CV_64F);
	xy = Mat::zeros(3, 1, CV_64F);
	xy.at<double>(2,0)=1;

	shear.at<double>(0,0)=1;
	shear.at<double>(0,1)=x;
	shear.at<double>(1,0)=y;
	shear.at<double>(1,1)=1;
	shear.at<double>(2,2)=1;

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
				xy.at<double>(0,0)=r;
				xy.at<double>(1,0)=c;
				double x, y;
				xy=shear*xy;
				x=xy.at<double>(0,0);
				y=xy.at<double>(1,0);
/*				if((r==image.rows/2&&c==image.cols/4)
						||(r==image.rows/4&&c==image.cols/2)
						   ||(r==image.rows/8&&c==image.cols/8)){
					cout<<x<<"->"<<r<<" "<<y<<"->"<<c<<endl;
				}
*/				if(x>=image.rows||y>=image.cols||x<=0||y<=0){
						image2.at<uint8_t>(r, c)=0;
					}
					else{
						image2.at<uint8_t>(r, c) = interpolation(image, x, y, r, c, option);
					}
		}
	}
//	cout<<shear<<shear.inv();
	return image2;
}

Mat negatives(Mat image){
	Mat image2;
	image2 = Mat::ones(image.rows, image.cols, CV_8UC1);
	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			for(int i=0; i<3; i++){
				image2.at<uint8_t>(r,c)=255-image.at<uint8_t>(r,c);
			}
		}
	}
	return image2;
}

Mat gamma(Mat image, double G){
	Mat image2;
	double div=pow(255, 1/G);
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	double x;
	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			x=(double)image.at<uint8_t>(r,c);
			image2.at<uint8_t>(r,c)=(pow(x, 1/G)/div)*(double)255;
		}
	}
	return image2;
}

Mat log_trans(Mat image){
	Mat image2;
	double mul=255/log(1+255);
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			image2.at<uint8_t>(r,c)=mul*((double)log(image.at<uint8_t>(r,c)+1));
			if(mul*((double)log(image.at<uint8_t>(r,c)+1))>255){
				image2.at<uint8_t>(r,c)=255;
			}
		}
	}
	return image2;
}

Mat contrast(Mat image){
	Mat image2;
	uint8_t max=0, min=255;
	image2 = Mat::ones(image.rows, image.cols, CV_8UC1);
	for(int r=2; r<image2.rows-2; r++){
		for(int c=2; c<image2.cols-2; c++){
			if(max<image.at<uint8_t>(r,c)){
				max=image.at<uint8_t>(r,c);
			}
			if(min>image.at<uint8_t>(r,c)){
				min=image.at<uint8_t>(r,c);
			}
		}
	}

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			image2.at<uint8_t>(r,c)=((image.at<uint8_t>(r,c)-min)*255/(max-min));
		}
	}
	return image2;
}

Mat thresholding(Mat image, int threshold){
	string str="/home/paras/p.jpg";
	Mat image2;
	image2 = Mat::ones(image.rows, image.cols, CV_8UC1);
	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			if(image.at<uint8_t>(r,c)<threshold){
				image2.at<uint8_t>(r,c)=0;
			}
			else{
				image2.at<uint8_t>(r,c)=255;
			}
		}
	}
	return image2;
}

Mat intensity_slicing(Mat image, double low, double high){
	Mat image2;
	image2 = Mat::ones(image.rows, image.cols, CV_8UC1);
	for(int r=2; r<image2.rows-2; r++){
		for(int c=2; c<image2.cols-2; c++){
			if(image.at<uint8_t>(r,c)<high&&image.at<uint8_t>(r,c)>low){
				image2.at<uint8_t>(r,c)=image.at<uint8_t>(r,c);
			}
			else{
				image2.at<uint8_t>(r,c)=0;
			}
		}
	}
	return image2;
}

Mat bit_slice(Mat image, int plane){
	Mat image2;
	uint8_t tmp;
	image2 = Mat::ones(image.rows, image.cols, CV_8UC1);
	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			tmp=(uint8_t)pow(2,plane-1);
			tmp=image.at<uint8_t>(r,c) & tmp;
			if(tmp){
				image2.at<uint8_t>(r,c)=255;
			}
			else{
				image2.at<uint8_t>(r,c)=0;
			}
		}
	}
	return image2;
}

Mat tie(Mat image, double x1, double y1, double x2, double y2, double x3, double y3
		, double x4, double y4, double x5, double y5, double x6, double y6
		, int option){
	string str="/home/paras/q.jpg";
	Mat image2, xy, xy2, restore, points;
/*	double x1=216.8, y1=64, x2=588.2, y2=256, x3=0, y3=0,
		   x4=140, y4=64, x5=281, y5=256, x6=0, y6=0;
*/
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	xy = Mat::zeros(3, 3, CV_64F);
	restore = Mat::zeros(3, 3, CV_64F);
	xy2 = Mat::zeros(3, 3, CV_64F);
	points = Mat::zeros(3, 1, CV_64F);
	points.at<double>(2,0)=1;

	xy.at<double>(0,0)=x1;
	xy.at<double>(0,1)=x2;
	xy.at<double>(0,2)=x3;
	xy2.at<double>(0,0)=x4;
	xy2.at<double>(0,1)=x5;
	xy2.at<double>(0,2)=x6;

	xy.at<double>(1,0)=y1;
	xy.at<double>(1,1)=y2;
	xy.at<double>(1,2)=y3;
	xy2.at<double>(1,0)=y4;
	xy2.at<double>(1,1)=y5;
	xy2.at<double>(1,2)=y6;

	xy.at<double>(2,0)=1;
	xy.at<double>(2,1)=1;
	xy.at<double>(2,2)=1;
	xy2.at<double>(2,0)=1;
	xy2.at<double>(2,1)=1;
	xy2.at<double>(2,2)=1;

	restore=xy*(xy2.inv());
//	cout<<restore<<endl;
	restore=restore.inv();
//	cout<<restore<<endl;

	for(int r=0; r<image.rows; r++){
		for(int c=0; c<image.cols; c++){
				points.at<double>(0,0)=r;
				points.at<double>(1,0)=c;
				double x, y;
				points=restore*points;
				x=points.at<double>(0,0);
				y=points.at<double>(1,0);
					if(x>=image.rows||y>=image.cols||x<=0||y<=0){
						image2.at<uint8_t>(r, c)=0;
					}
					else{
						image2.at<uint8_t>(r, c) = interpolation(image, x, y, r, c, option);
					}
		}
	}
	return image2;
}

Mat histogram_equal(Mat image){
	Mat image2;
	double intensities[256]={0};
	double pi;
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	long long int size=image.rows*image.cols;
	for(int r=0; r<image.rows; r++){
		for(int c=0; c<image.cols; c++){
			intensities[image.at<uint8_t>(r,c)]+=1;
		}
	}
	for(int i=1; i<256; i++){
		intensities[i]+=intensities[i-1];
	}
	for(int i=0; i<256; i++){
		pi=intensities[i];
		pi=(pi/size)*255;
		intensities[i]=pi;
		pi=(int)intensities[i];
		if(intensities[i]-pi>.5f){
			intensities[i]=pi+1;
		}
		else{
			intensities[i]=pi;
		}
		if(intensities[i]>=255){
			intensities[i]=255;
		}
	}
	for(int r=0; r<image.rows; r++){
		for(int c=0; c<image.cols; c++){
			image2.at<uint8_t>(r,c)=intensities[image.at<uint8_t>(r,c)];
		}
	}
	return image2;
}

Mat adaptive_histo(Mat image){
	int masksize=50;
	Mat image2;
	double intensities[256]={0};
	double pi;
	long long size;
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	if(masksize%2==0){
		masksize++;
	}
	for(int r=0; r<image.rows; r++){
		for(int c=0; c<image.cols; c++){
				size=0;
				for(int i=0; i<256; i++){
					intensities[i]=0;
				}
				for(int k=r-masksize/2; k<r+masksize/2; k++){
					for(int l=c-masksize/2; l<c+masksize/2; l++){
						if(k>=0&&l>=0&&k<image.rows&&l<=image.cols){
							intensities[image.at<uint8_t>(k,l)]+=1;
						}
						size++;
					}
				}
				for(int i=1; i<256; i++){
					intensities[i]+=intensities[i-1];
				}
				for(int i=0; i<256; i++){
					pi=intensities[i];
					pi=(pi/size)*255;
					intensities[i]=pi;
					pi=(int)intensities[i];
					if(intensities[i]-pi>.5f){
						intensities[i]=pi+1;
					}
					else{
						intensities[i]=pi;
					}
					if(intensities[i]>=255){
						intensities[i]=255;
					}
		//		cout<<intensities[i]<<endl;
			  }
			image2.at<uint8_t>(r,c)=intensities[image.at<uint8_t>(r,c)];
		}
	}
	cout<<"Image Ready!"<<endl;
	return image2;
}

Mat histogram_match(Mat image, Mat image3){
	string str2="/home/paras/ME3.jpg";
	string str="/home/paras/ME5.jpg";
	Mat image2;
	double intensities[256]={0};
	double pi;
	double intensities2[256]={0};
	double pi2;
	double intensities3[256]={-1};
	int max1=0, min1=255, max2=0, min2=255, tmp;
	long long sum=0;
	image2 = Mat::zeros(image.rows, image.cols, CV_8UC1);
	int size=image.rows*image.cols;
	int size2=image3.rows*image3.cols;
	for(int r=0; r<image.rows; r++){
		for(int c=0; c<image.cols; c++){
			intensities[image.at<uint8_t>(r,c)]+=1;
			if(max2<image.at<uint8_t>(r,c)){
				max2=image.at<uint8_t>(r,c);
			}
			if(min2>image.at<uint8_t>(r,c)){
				min2=image.at<uint8_t>(r,c);
			}
		}
	}
	for(int r=0; r<image3.rows; r++){
		for(int c=0; c<image3.cols; c++){
			intensities2[image3.at<uint8_t>(r,c)]+=1;
		}
	}
	for(int i=0; i<256; i++){
		intensities[i]+=intensities[i-1];
		intensities2[i]+=intensities2[i-1];
	}
	for(int i=0; i<256; i++){
		pi=intensities[i];
		pi=(pi*255)/size;
		intensities[i]=pi;
		pi=(int)intensities[i];
		if(intensities[i]-pi>.5f){
			intensities[i]=pi+1;
		}
		else{
			intensities[i]=pi;
		}
		pi2=intensities2[i];
		pi2=(pi2*255)/size2;
		intensities2[i]=pi2;
		pi2=(int)intensities2[i];
		if(intensities2[i]-pi2>.5f){
			intensities2[i]=pi2+1;
		}
		else{
			intensities2[i]=pi2;
		}
		if(intensities[i]>=255){
			intensities[i]=255;
		}
		if(intensities2[i]>=255){
			intensities2[i]=255;
		}
	}
	for(int i=0; i<256; i++){
        int j = 0;
        do {
              intensities3[i] = j;
              j++;
        }while(intensities[i] > intensities2[j]);

	}

	for(int i=0; i<256; i++){
		if(max1<intensities3[i]){
			max1=intensities3[i];
		}
		if(min1>intensities3[i]&&intensities3[i]!=-1){
			min1=intensities3[i];
		}
	}
//cout<<"     "<<max<<" "<<min<<" "<<max2<<" "<<min2<<endl;
//for(int i=0; i<256; i++)
//	cout<<i<<" "<<intensities[i]<<" "<<intensities2[i]<<" "<<intensities3[i]<<endl;

	for(int r=0; r<image2.rows; r++){
		for(int c=0; c<image2.cols; c++){
			if(intensities3[image.at<uint8_t>(r,c)]==-1){
//				tmp=image.at<uint8_t>(r,c);
				tmp=0;
			}
			else{
				tmp=intensities3[(int)image.at<uint8_t>(r,c)];
//				if(image.at<uint8_t>(r,c)==255){
//					cout<<"a="<<(uint8_t)image.at<uint8_t>(r,c)<<"c="<<tmp<<endl;
//				}
			}
//			tmp=((tmp-min1)*(max2-min2)/(max1-min1));
			image2.at<uint8_t>(r,c)=tmp;
			sum+=min((int)tmp,(int)intensities2[(int)image.at<uint8_t>(r,c)]);
//			sum+=tmp;
		}
	}
//	cout<<"sum="<<sum<<endl;
	return image2;
}

Mat ask_path(){
	string input_path;
	cout<<"Enter image path: ";
	cin>>input_path;
//input_path="/home/paras/ME5.jpg";
	Mat image;
	image = imread(input_path, CV_LOAD_IMAGE_GRAYSCALE);
	if(image.empty()){
		cout<<"Cannot load or find image\n";
	}
	return image;
}

Mat ask_paths(){
	string input_path;
	cout<<"Enter second image path: ";
	cin>>input_path;
//input_path="/home/paras/ME3.jpg";
	Mat image;
	image = imread(input_path, CV_LOAD_IMAGE_GRAYSCALE);
	if(image.empty()){
		cout<<"Cannot load or find image\n";
	}
	return image;
}

void print_image(Mat output){
	string path;
	cout<<"Enter output image path: ";
	cin>>path;
//path="/home/paras/out.jpg";
	imwrite(path, output);
}

void show_image(Mat image, Mat image2){
	namedWindow( "Input Image", WINDOW_AUTOSIZE);
	namedWindow( "Output Image", WINDOW_AUTOSIZE);
	imshow( "Input Image", image );
	imshow( "Output Image", image2 );

}

void rmse(Mat my, Mat inbuilt){
	double x=0, y;
	long long size=my.rows*my.cols;
	for(int i=2; i<my.rows-2; i++){
		for(int j=2; j<my.cols-2; j++){
		x+=(double)pow((inbuilt.at<uint8_t>(i,j)-my.at<uint8_t>(i,j)), 2)/size;
		}
	}
	y=sqrt(x);
	cout<<"RMSE="<<y<<endl;
	if(x){
		cout<<"PSNR="<<10*log10((double)(255*255)/y)<<endl;
	}
}

int main(){
	int option=1; int repeat=1; int save=0;
	while(repeat==1){
		cout<<"..........SELECT IMAGE TRANSFORMATION...........\n"<<endl;
		cout<<"1.  IMAGE RESIZE"<<endl;
		cout<<"2.  ROTATION"<<endl;
		cout<<"3.  TRANSLATION"<<endl;
		cout<<"4.  SHEAR"<<endl;
		cout<<"5.  IMAGE NEGATIVES"<<endl;
		cout<<"6.  LOG TRANSFORMATION"<<endl;
		cout<<"7.  POWER LAW"<<endl;
		cout<<"8.  CONTRAST STREACHING"<<endl;
		cout<<"9.  THRESHOLDING"<<endl;
		cout<<"10. INTESITY SLICING"<<endl;
		cout<<"11. BITPLANE SLICING"<<endl;
		cout<<"12. IMAGE RECONSTRUCTION FROM TIE POINTS"<<endl;
		cout<<"13. HISTOGRAM EQUALIZATION"<<endl;
		cout<<"14. ADAPTIVE HISTOGRAM EQUALIZATION"<<endl;
		cout<<"15. HISTOGRAM MATCHING"<<endl;
		cout<<"\nEnter your choice: ";
		cin>>option;
		Mat input, output, inbuiltOutput;
		int interpo=2;
		if(option==1){
			input=ask_path();
			if(!input.empty()){
				double mX, mY;
				cout<<"Enter horizontal and vertical scaling factors:";
				cout<<"\n(EX: 3 .5 for 3 times horizontal and 1/2 times vertical):\n";
				cin>>mY>>mX;
				cout<<"Press 1 for nearest neighbor or 2 for bilinear interpolation:\n";
				cin>>interpo;
				output=image_resize(input, mX, mY, interpo);
				show_image(input, output);
				Size s=output.size();
				if(interpo==1){
					resize(input, inbuiltOutput, s, 1, 1, INTER_NEAREST);
				}
				else{
					resize(input, inbuiltOutput, s, 1, 1, INTER_LINEAR);
				}
				rmse(output, inbuiltOutput);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==2){
			input=ask_path();
			if(!input.empty()){
				double angle;
				cout<<"Enter angle in degrees clockwise:\n";
				cin>>angle;
				cout<<"Press 1 for nearest neighbor or 2 for bilinear interpolation:\n";
				cin>>interpo;
				output=rotation(input, angle, interpo);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}

			}
		}
		else if(option==3){
			input=ask_path();
			if(!input.empty()){
				double mX, mY;
				cout<<"Enter horizontal and vertical translating percentage";
				cout<<"\n(EX: 60 -70 for 60% left and 70% below):\n";
				cin>>mY>>mX;
				output=translation(input, mX, mY);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==4){
			input=ask_path();
			if(!input.empty()){
				double mX, mY;
				int choice;
				cout<<"Enter 1 for vertical or 2 for horizontal shearing\n";
				cin>>choice;
				cout<<"Enter shearing percentage";
				if(choice!=1){
					cout<<"\n(EX: 60 for 60% left or -70 for 70% right)\n";
					cin>>mY;
					mX=0;
				}
				else{
					cout<<"\n(EX: 60 for 60% top or -70 for 70% below)\n";
					cin>>mX;
					mY=0;
				}
				cout<<"Press 1 for nearest neighbor or 2 for bilinear interpolation:\n";
				cin>>interpo;
				output=shearing(input, mX, mY, interpo);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==5){
			input=ask_path();
			if(!input.empty()){
				output=negatives(input);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==6){
			input=ask_path();
			if(!input.empty()){
				output=log_trans(input);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==7){
			input=ask_path();
			if(!input.empty()){
				double G;
				cout<<"Enter value of gamma:\n";
				cin>>G;
				output=gamma(input, G);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==8){
			input=ask_path();
			if(!input.empty()){
				output=contrast(input);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==9){
			input=ask_path();
			if(!input.empty()){
				double threshold;
				cout<<"Enter thresholding value (between 0 to 255):\n";
				cin>>threshold;
				output=thresholding(input, threshold);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==10){
			input=ask_path();
			if(!input.empty()){
				int low, high;
				cout<<"Enter lower and upper intensity range (between 0 to 255):\n";
				cin>>low>>high;
				output=intensity_slicing(input, low, high);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==11){
			input=ask_path();
			if(!input.empty()){
				int plane;
				cout<<"Enter a plane between 1 to 8 (lower values are noisier)\n";
				cin>>plane;
				output=bit_slice(input, plane);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==12){
			cout<<"Input format:\n";
			cout<<"x1(input) x3(output) y1(input) y3(output)\n";
			cout<<"Translational(like rotation about center) correction\n";
			cout<<"requires 3 pairs of input output points\n";
			cout<<"But for non Translational transformations like Shearing\n";
			cout<<"correction requires only 2 pairs input output points\n";
			cout<<"the last points should be (0,0) mapped to (0,0)\n";
			cout<<"Example 1 for centrally rotated image q1.jpg\n";
			cout<<"561.577 128 -29.8092 128 348.804 257 317.127 514 710.873 514 348.804 257\n";
			cout<<"Example 2 for vertically sheared image q.jpg\n";
			cout<<"216.8 140 64 64 588.2 281 256 256 0 0 0 0\n";
			cout<<"To run Example-1 type q1.jpg or q.jpg for Example-2\n";
			input=ask_path();
			if(!input.empty()){
				double x1=216.8, y1=64, x2=588.2, y2=256, x3=0, y3=0,
					   x4=140, y4=64, x5=281, y5=256, x6=0, y6=0;
				cout<<"Copy and paste 12 number input line corresponding to the selected input file:\n";
				cin>>x1>>x4>>y1>>y4>>x2>>x5>>y2>>y5>>x3>>x6>>y3>>y6;
				cout<<"Press 1 for nearest neighbor or 2 for bilinear interpolation:\n";
				cin>>interpo;
				output=tie(input, x1, y1, x2, y2, x3, y3
						, x4, y4, x5, y5, x6, y6, interpo);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==13){
			input=ask_path();
			if(!input.empty()){
				output=histogram_equal(input);
				show_image(input, output);
				equalizeHist(input, inbuiltOutput);
				rmse(input, inbuiltOutput);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==14){
			cout<<"Image quality should be low otherwise it may take long time\n";
			input=ask_path();
			if(!input.empty()){
				output=adaptive_histo(input);
				show_image(input, output);
				cout<<"Close the image window(s) to proceed\n";
				waitKey(0);
				cout<<"Press 1 to save or any other number to discard\n";
				cin>>save;
				if(save==1){
					print_image(output);
				}
			}
		}
		else if(option==15){
			cout<<"IMAGE TO BE TRANSFORMED\n";
			input=ask_path();
			if(!input.empty()){
				Mat input2;
				cout<<"IMAGE WHOSE HISTOGRAM IS TO MATCHED\n";
				input2=ask_paths();
				if(!input2.empty()){
					output=histogram_match(input, input2);
					show_image(input, output);
					inbuiltOutput=input.clone();
					equalizeHist(inbuiltOutput, input2);
					rmse(input, inbuiltOutput);
					cout<<"Close the image window(s) to proceed\n";
					waitKey(0);
					cout<<"Press 1 to save or any other number to discard\n";
					cin>>save;
					if(save==1){
						print_image(output);
					}
				}
			}
		}
		else{
			cout<<"Wrong option!!!!!\n";
		}
		cout<<"Press 1 to continue or any other number to exit:\n";
		cin>>repeat;
		if(repeat!=1){
			break;
		}
	}
	cout<<"Program Closed!\n";
	return 0;
}
