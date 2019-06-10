#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdlib>
#include <queue>
#include <stack> 
#include <cmath>

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
    
    if(argc!=4){
        cout<<"Usage: ../seg input_image initialization_file output_mask"<<endl;
        return -1;
    }
    

    Mat in_image;
    in_image = imread(argv[1]);
    

    Mat out_image = in_image.clone();
	
	
	if(!in_image.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }

    if(in_image.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
        return -1;
    }
    

    int width = in_image.cols;
    int height = in_image.rows;
    int minf;
    int minb;
    int fcount,bcount=0;
    

    ifstream f(argv[2]);
    
    if(!f){
        cout<<"Could not load initial mask file!!!"<<endl;
        return -1;
    }
	
    int n;
    f>>n;
       
    vector<Vec3b> fPoints(n);
    vector<Vec3b> bPoints(n);
    vector<int> source_points;
    vector<int> target_points;
    
    vector<bool> visited;
    int total_count=height*width;
    
    int path_count=total_count+2;
    
    vector<int> path (path_count);
    
    vector < vector < pair < int, int > > > imagevector(total_count);
    
    int loc=0;
    int location[total_count];
    Vec3b pixelw,pixelwr,pixelwd,pixelwl,pixelwu;
   

    for(int i=0;i<n;++i){
        int x, y, t;
        f>>x>>y>>t;
        

        if(x<0 || x>=width || y<0 || y>=height){
            cout<<"I valid pixel mask!"<<endl;
            return -1;
        }
        

        Vec3b configintensity = in_image.at<Vec3b>(y, x);
        
        
        if(t==1){
            

            fPoints[fcount]=configintensity;
           
            
            target_points.push_back((width * y) + x);
            fcount++;
        } else {
            

            bPoints[bcount]=configintensity;
           
            source_points.push_back((width * y) + x);
            bcount++;
        }
        

        

    }
    
    

    int sumf[fcount];
    int sumb[bcount];
    

    for(int k=0;k<height;k++){
        

        for(int l=0;l<width;l++){
            

            Vec3b pixel= in_image.at<Vec3b>(k, l);
            for(int p=0;p<fcount;p++){
                

                sumf[p]=sqrt(pow(int(fPoints[p][0])-int(pixel[0]),2)+pow(int(fPoints[p][1])-int(pixel[1]),2)+pow(int(fPoints[p][2])-int(pixel[2]),2));
                                

            }
            for(int q=0;q<bcount;q++){
                

                sumb[q]=sqrt(pow(int(bPoints[q][0])-int(pixel[0]),2)+pow(int(bPoints[q][1])-int(pixel[1]),2)+pow(int(bPoints[q][2])-int(pixel[2]),2));
                

            }
            

            minf = sumf[0];
            

            for (int i=0; i <fcount;i++)
            {
                if (minf > sumf[i]){
                    minf = sumf[i];
                }
            }
            minb = sumb[0];
            

            for (int i=0; i <bcount;i++)
            {
                if (minb > sumb[i]){
                    minb = sumb[i];
                }
            }
            

            if(minf<minb){
                pixel[0]=255;
                pixel[1]=255;
                pixel[2]=255;
                

            }
            else{
                pixel[0]=0;
                pixel[1]=0;
                pixel[2]=0;
            }
            

            out_image.at<Vec3b>(k, l) = pixel;
        }
        
    }
    

    imwrite( argv[3], out_image);
	
	cout<<"weight assigning start"<<endl;
    
    for (int h=0;h<height;h++){
        

        for (int w = 0; w < width;w++) {
            

            loc=(h*width)+w;
            pixelw=out_image.at<Vec3b>(h, w);
                      

            if(loc==0){
                    

                pixelwd=out_image.at<Vec3b>(h+1, w);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 && 
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 && 
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
				
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight2);                   

                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    
                    pair < int, int > adjweight1 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                               

            }
            else if(loc==(width-1)){
                

                //cout<<"loc upper last width:"<<loc<<endl;
                

                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwd=out_image.at<Vec3b>(h+1,w);
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight1);
                }
                
               

            }
            else if(loc==(((width*height)-1)-(width-1))){
                

                //cout<<"loc lower left index:"<<loc<<endl;
                

                pixelwu=out_image.at<Vec3b>(h-1, w);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                

                if((int(pixelw.val[0])-int(pixelwu.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwu.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwu.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                                

            }
            else if(loc==((width*height)-1)){
                

                //cout<<"loc last index :"<<loc<<endl;
                

                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwu=out_image.at<Vec3b>(h-1,w);
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight1);
                }
                

            }
            else if(loc>0 && loc<width-1){
                

               // cout<<"loc upper range :"<<loc<<endl;
                

                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                pixelwd=out_image.at<Vec3b>(h+1, w);
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight3 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight3);
                }
                else{
                    pair < int, int > adjweight3 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight3);
                }
                               

            }
            else if(h==height-1 && w!=0 && w!=width-1){
                

                //cout<<"loc lower range :"<<loc<<endl;
                

                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                pixelwu=out_image.at<Vec3b>(h-1, w);
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                

                if((int(pixelw.val[0])-int(pixelwu.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwu.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwu.val[2]))==0){
                    

                    pair < int, int > adjweight3 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight3);
                }
                else{
                    pair < int, int > adjweight3 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight3);
                }
                

            }
            else if(h!=height-1 && w==0 && h!=0){
                

                

               // cout<<"loc left line range:"<<loc<<endl;
                

                pixelwd=out_image.at<Vec3b>(h+1, w);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                pixelwu=out_image.at<Vec3b>(h-1, w);
                

                if((int(pixelw.val[0])-int(pixelwu.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwu.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwu.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight3 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight3);
                }
                else{
                    pair < int, int > adjweight3 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight3);
                }
            }
            

            else if(h!=height-1 && w==width-1 && h!=0){
                

                //cout<<"loc right line range:"<<loc<<endl;
                

                pixelwd=out_image.at<Vec3b>(h+1, w);
                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwu=out_image.at<Vec3b>(h-1, w);
                

                if((int(pixelw.val[0])-int(pixelwu.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwu.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwu.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                else{
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight2);
                    

                }
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight1 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight1);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                
                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    
                    pair < int, int > adjweight3 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight3);
                }
                else{
                    pair < int, int > adjweight3 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight3);
                }
                               
            }
            

            else{
                

                pixelwd=out_image.at<Vec3b>(h+1, w);
                pixelwl=out_image.at<Vec3b>(h, w-1);
                pixelwu=out_image.at<Vec3b>(h-1, w);
                pixelwr=out_image.at<Vec3b>(h, w+1);
                

                if((int(pixelw.val[0])-int(pixelwu.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwu.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwu.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-width,77777777);
                    imagevector[loc].push_back(adjweight2);
                    
                }
                else{
                    
                    pair < int, int > adjweight2 = make_pair(loc-width,7);
                    imagevector[loc].push_back(adjweight2);
                    
                }
                

                if((int(pixelw.val[0])-int(pixelwl.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwl.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwl.val[2]))==0){
                    

                    pair < int, int > adjweight2 = make_pair(loc-1,77777777);
                    imagevector[loc].push_back(adjweight2);
                }
                else{
                    pair < int, int > adjweight1 = make_pair(loc-1,7);
                    imagevector[loc].push_back(adjweight1);
                }
                

                if((int(pixelw.val[0])-int(pixelwr.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwr.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwr.val[2]))==0){
                    

                    pair < int, int > adjweight3 = make_pair(loc+1,77777777);
                    imagevector[loc].push_back(adjweight3);
                }
                else{
                    pair < int, int > adjweight3 = make_pair(loc+1,7);
                    imagevector[loc].push_back(adjweight3);
                }
                

                if((int(pixelw.val[0])-int(pixelwd.val[0]))==0 &&
				(int(pixelw.val[1])-int(pixelwd.val[1]))==0 &&
				(int(pixelw.val[2])-int(pixelwd.val[2]))==0){
                    

                    pair < int, int > adjweight4 = make_pair(loc+width,77777777);
                    imagevector[loc].push_back(adjweight4);
                }
                else{
                    pair < int, int > adjweight4 = make_pair(loc+width,7);
                    imagevector[loc].push_back(adjweight4);
                }
                
            }
            
        }
       
    }
	
	cout<<"weight assigning finished";

    imagevector.resize(path_count);
    
    for(int i=0;i<source_points.size();i++ )
    {
        pair < int, int > adjweight1 = make_pair(source_points[i],77777777);
        imagevector[total_count].push_back(adjweight1);
        pair < int, int > adjweight2 = make_pair(total_count,77777777);
        imagevector[source_points[i]].push_back(adjweight2);
        

    }
       
    for(int i=0;i<target_points.size();i++ )
    {
        pair < int, int > adjweight1 = make_pair(target_points[i],77777777);
        imagevector[total_count+1].push_back(adjweight1);
        pair < int, int > adjweight2 = make_pair(total_count+1,77777777);
        imagevector[target_points[i]].push_back(adjweight2);
        
    }
    
    int s =total_count;
    int t =total_count+1;
    int astop=1;
       
	cout<<"dfs starting";
    
    while(astop==1) {
        
	vector<int> visitnode (path_count);
        //int visitnode[path_count];
        for(int i=0;i<path_count;i++){
            visitnode[i]=0;
        }
        

        queue <int> blckq;
        blckq.push(s);
        visitnode[s] = 1;
        path[s] = -1;
        int min_flowweight = 77777790;
        bool flag= 0;
        int a=1;
		
            while(!flag && (visitnode[t]==0)) {

                    int parent = blckq.front();
                    blckq.pop();
                    int i=0;
                    while(i < imagevector[parent].size()){
                      //  cout<<"i is "<<i<<endl;
                        if (imagevector[parent][i].second > 0 && visitnode[imagevector[parent][i].first]==0)
                        {
                            blckq.push(imagevector[parent][i].first);
                            path[imagevector[parent][i].first] = parent;
                            visitnode[imagevector[parent][i].first] = 1;
                        }
                        i++;
                    }

            flag = blckq.empty();

                if(visitnode[t]==1){

                break;
            }
             
            } 

            if(visitnode[t]==1){
                

                astop=1;
               
                
                for (int root=t; root!=s; root=path[root])
                {
                    
                    int flowweight=0;
                    int child = path[root];
					
					int k=0;
					while(k <imagevector[child].size()){                
                        if(imagevector[child][k].first==root){
                            flowweight = imagevector[child][k].second;
                        }
						k++;
                    }
                        if(flowweight<min_flowweight){
                            min_flowweight = flowweight;
                        }
                   
                }
                

                for (int root=t; root!=s; root=path[root])
                {
                    
                    int child = path[root];
					
                	int z=0;			
                    while( z< imagevector[child].size()){
                        if (imagevector[child][z].first == root){
                            
                            imagevector[child][z].second = imagevector[child][z].second - min_flowweight;
                            
                        }
						z++;
                    }
					
					int d=0;
                    while(d< imagevector[root].size()){
                        if(imagevector[root][d].first == child){
                            
                            imagevector[root][d].second = imagevector[root][d].second + min_flowweight;
                            

                        }
						d++;
                    }
                    

                }
            }
            else if(visitnode[t]==0){
				
                break;
            }
            
        } 
		
	
    for (int h=0;h<height;h++){
        
       for (int w = 0; w < width;w++) {   
	   
			Vec3b pixel = out_image.at<Vec3b>(h, w);	
				
			    pixel[0]=255;
                pixel[1]=255;
                pixel[2]=255;
				
			out_image.at<Vec3b>(h, w) = pixel;	
    
			
		}
	}
	
	imwrite( argv[3], out_image);


// dfs (depth first search)
	
        //int visitnode[path_count];
	vector<int> visitnode (path_count);
        for(int i=0;i<path_count;i++){
            visitnode[i]=0;
        }	
				
	    stack <int> stacknodes;
        stacknodes.push(s);		
        visitnode[s] = 1;
        path[s] = -1;
		
		int y_index = s / width;
		int x_index = s % width;
		
		Vec3b pixel_color;
		pixel_color.val[0] = 0;
		pixel_color.val[1] = 0;
		pixel_color.val[2] = 0;
		
		out_image.at < Vec3b > (y_index, x_index) = pixel_color;
		
	while(!stacknodes.empty()){
		
        int parent = stacknodes.top();      
		stacknodes.pop();
		
	    int i=0;
        while(i < imagevector[parent].size()){
                      
			if (visitnode[imagevector[parent][i].first]==0 && imagevector[parent][i].second > 0 )
               {
                    stacknodes.push(imagevector[parent][i].first);
					visitnode[imagevector[parent][i].first] = 1;
					
					Vec3b pixel_color;
					pixel_color.val[0] = 0;
					pixel_color.val[1] = 0;
					pixel_color.val[2] = 0;
					
					int y_index,x_index,adjact;
					adjact=imagevector[parent][i].first;
					
					y_index = adjact / width;
					x_index = adjact % width;
					
					out_image.at < Vec3b > (y_index, x_index) = pixel_color;
                         
               }
               i++;
         }
		
            }
			
	imwrite( argv[3], out_image);

    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
    waitKey(0);
    return 0;	
    
    }
