#include <iostream>
#include <fstream>
#include <math.h> 
using namespace std;


class Kmean{
public:

	class xyCoord{
	public:
		xyCoord(){};
		double X;
		double Y;
		int Label;
		xyCoord(double x, double y){
			X = x;
			Y = y;
		}
	};
	class Point{
	public:
		Point(){};
		int X;
		int Y;
		int Label;
		double Distance;
		Point(int x, int y){
			xyCoord xy(x,y);
			X = xy.X;
			Y = xy.Y;
			Distance = 9999.0;
		}
	};
	
	int K; 
	int numPts;
	Point* pointSet;
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int **imgAry;
	xyCoord* Kcentroids;
	int change;
	int iteration;

	Kmean(int r, int c, int min, int max, int k){
		numRows = r;
		numCols = c;
		minVal = min;
		maxVal = max;	
		K = k;
		imgAry = new int*[numRows];

		for (int i = 0; i < numRows; i++)
			imgAry[i] = new int[numCols];
		
		for (int x = 0; x < numRows; x++)
			for (int y = 0; y < numCols; y++)
				imgAry[x][y] = 0;

		Kcentroids = new xyCoord[K+1];
		numPts = 0;
		change = 0;
	}

	int extractPts(ifstream& in, ofstream& out){
		numPts = 0;
		int point;
		for(int r = 0; r < numRows; r++){
			for(int c = 0; c < numCols; c++){
				in >> point;
				if(point>0){
					out<<r<<" "<<c<<endl;
					numPts++;
				}
			}
		}
		return numPts;
	}

	void loadPointSet(ifstream& in2, Point pointSet[]){
		int i = 0,x,y;
		while(in2 >> x >> y && i < numPts){
			pointSet[i] = Point(x,y);
			i++;
		}
	}

	void assignLabel(Point pointSet[] , int K){
		int front = 0;
		int back = numPts-1;
		int label = 1;
		while(front <= back){
			if(label > K)
				label = 1;
			else if(label == K){
				pointSet[front].Label = label;
				front++;
				label++;
				pointSet[back].Label = 1;
				back--;
				// label++;
			}
			else{
				pointSet[front].Label = label;
				front++;
				label++;
				pointSet[back].Label = label;
				back--;
				label++;
			}	
		}
	}

	void Point2Image (Point pointSet[], int** imgAry){

		for (int i = 0; i < numPts; i++)
		{	
			imgAry[pointSet[i].X][pointSet[i].Y] = pointSet[i].Label;
		}	
	}

	void printImage(int** imgAry, ofstream& o2, int iteration){

		o2<<"*** Result of iteration "<< ++iteration<< " ****\n";
		for (int x = 0; x < numRows; x++)
		{
			for (int y = 0; y < numCols; y++)
			{	
				if(imgAry[x][y] > 0){
					o2<<imgAry[x][y];
				}	
				else{
					o2<<" ";
				}
			}
			o2<<endl;
		}
	}

	void computeCentroids(Point pointSet[], xyCoord Kcentroids[]){

		int *sumX = new int[K+1];
		int *sumY = new int[K+1];
		int *totalPt = new int[K+1];
		for(int i = 1 ; i<K+1; i++){
			sumX[i]=0;
			sumY[i]=0;
			totalPt[i]=0;
		}

		int index = 0, label;

		while(index < numPts){
			label = pointSet[index].Label;
			sumX[label] += pointSet[index].X;
			sumY[label] += pointSet[index].Y;
			totalPt[label]++;
			index++;
		}
		label = 1;
		while(label <= K){
			if(totalPt[label] == 0){
				Kcentroids[label].X = sumX[label];
				Kcentroids[label].Y = sumY[label];
				break;
			}
			Kcentroids[label].X = sumX[label]/totalPt[label];
			Kcentroids[label].Y = sumY[label]/totalPt[label];
			label++;
		}
	}

	double computeDist(Point p1, xyCoord p2){
		double differenceX,differenceY;
		differenceX = (p2.X - p1.X)*(p2.X - p1.X);
		differenceY = (p2.Y - p1.Y)*(p2.Y - p1.Y);
		return sqrt(differenceX + differenceY);
	}

	void distanceMinLabel(Point &p, xyCoord Kcentroids[]){

		double minDist = 99999.0;
		int minLabel = 0;

		int label = 1;
		while(label <= K){
			double distance = computeDist(p,Kcentroids[label]);
			if(distance < minDist){
				minLabel = label;
				minDist = distance;
			}
			label++;
		}

		if(minDist < p.Distance){
			p.Distance = minDist;
			p.Label = minLabel;
			change++;
		}
	}

	void writePtSet(Point pointSet[], ofstream& o3){
		o3<<numPts<<endl;
		o3<<numRows<<" "<<numCols<<endl;
		for (int i = 0; i < numPts; ++i)
		{
			o3<<pointSet[i].X<<" "<<pointSet[i].Y<<" "<<pointSet[i].Label<<endl;
		}
	}

	void kMeansClustering(Point pointSet[] , int K, ofstream& o2){
		iteration = 0;
		assignLabel(pointSet, K);
		while(true){
			Point2Image(pointSet, imgAry); 
			printImage(imgAry, o2, iteration);
			
			change = 0;

			computeCentroids(pointSet, Kcentroids);

			int index = 0;
			
			while(index < numPts){
				distanceMinLabel(pointSet[index],Kcentroids);
				index++;	
			}
			if(change==0){
				break;
			}
			iteration++;
		}	
	}
};





int main(int argc, char const *argv[])
{
	ifstream in;
	ofstream o1,o2,o3;
	in.open(argv[1]);

	int numRows,numCols,minVal,maxVal,K;
	in >> numRows;
	in >> numCols;
	in >> minVal;
	in >> maxVal;

	o1.open(argv[2]);
	o2.open(argv[3]);
	o3.open(argv[4]);
	string o1_name = argv[2];
	cout<<"enter value of K: ";
	cin >> K;

	Kmean km(numRows,numCols,minVal,maxVal,K);

	int numPts = km.extractPts(in,o1);
	km.numPts = numPts;
	km.pointSet = new Kmean::Point[numPts];

	o1.close();

	ifstream in2;
	in2.open(o1_name);

	km.loadPointSet(in2, km.pointSet);
	km.kMeansClustering(km.pointSet, K,o2);
	km.writePtSet(km.pointSet,o3);

	in.close();
	in2.close();
	o2.close();
	o3.close();

	return 0;
}