#include <iostream>
#include <vector>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h> 
#include <set>    
#include <cmath>
#include <chrono>
#include <limits.h>
#include <cfloat>
#include<fstream> 

using namespace std;





//Struct for points
struct Point{
    //x and y coordinates for point
    double x;
    double y;
    //custom operator to check if two points are the same
    bool operator==(const Point& a){
        return ((this->x == a.x)&&(this->y == a.y));
    }

};
//Struct for lines
struct Line{
    //slope and y-intercept for line
    double slope;
    double y;

    //Calculates slope and y-intercept given two points
    void calLine(Point v1, Point v2){
        //special case for when two points are on a vertical line
        if(v1.x == v2.x){
            slope = DBL_MAX;
            y = 0;  
        }
        //special case for when the two points are on a horizontal line
        if(v1.y == v2.y){
            slope = 0;
            y = v1.y;
        }

        //basic formula for calculating slope and y-intercept
        double s = v1.y - v2.y;
        s = s/(v1.x - v2.x);
        y = v1.x * s;
        y = v1.y - y;
        slope = s;
    }
};

//Takes a vector of points, will be convex hull, and removes any points 
//that are on a line created by two other points. Will keep the two farthest points on the line and only remove
//points that don't change the convex hull.
void removeExtraPoints(vector<Point>& vec){
    //keeps track of the first point on the current line you are looking at.
    int ind = 0;
    //slope and y-intercept of the line created by the last two points looked at.
    double oldS = DBL_MIN;
    double oldY = DBL_MIN;
    for(int i = 0; i < vec.size()-1; i++){
        Line line;
        //creates a line for the ith and (i + 1)th value in the convex hull list
        line.calLine(vec[i],vec[i+1]);
        //if the new indices create a new line from the last two then reset the variables above.
        if(oldS != line.slope || oldY != line.y){
            oldS = line.slope;
            oldY = line.y;
            ind = i;
        }else{
            //if the line is the same remove the value at the ith spot in the vector.
            if(i - ind >= 1){
                vec.erase(vec.begin() + i);
                i--;
            }
        }
        

    }
    //special case for the line which is created by the last and first value in the vector.
    Line line;
    line.calLine(vec[vec.size()-1],vec[0]);
    if(oldS == line.slope && oldY == line.y){
        vec.erase(vec.begin() + vec.size()-1);
    }else{
        return;
    }
    line.calLine(vec[0],vec[1]);
    while(oldS == line.slope && oldY == line.y){
            vec.erase(vec.begin());
            line.calLine(vec[0],vec[1]);
    }
}
// returns the distance between two points.
double calcDistance(double p1X, double p1Y , long double p2X, long double p2Y){
    double ans = ((p1X - p2X) * (p1X - p2X)) + ((p1Y - p2Y) * (p1Y - p2Y));
    ans = sqrt(ans);

    return ans;
    
}
//Finds the minimum distance between the line connecting points v1 and v2 and another point.
double findDistance(Point v1, Point v2 , Point point){
    //special case for when v1 and v2 are connected by a vertical line
    if(v1.x == v2.x){
        return abs(point.x - v1.x);
    }
    //special case for when v1 and v2 are connected by a horizontal line
    if(v1.y == v2.y){
        return abs(point.y - v2.y);
    }

    //uses long double to reduce rounding error.
    //calculates line and its perpendicular line to get the minimum distance.
    long double s = (v1.y - v2.y)/(v1.x - v2.x);
    long double y = v1.y - (v1.x * s);

    long double s1 = -1/s;
    long double y1 = point.x * s1;
    y1 = point.y - y1;

    long double s2 = s - s1;
    long double y2 = y1 - y;
    long double xAns = y2/s2;
    long double yAns = (s1 * xAns) + y1;

    double ans = calcDistance(point.x,point.y, xAns, yAns);

    return ans;
    
}
//Picks the closest point in vec to the line which goes through points v1 and v2
double pickPoint(vector<Point>& vec,Point v1, Point v2  ){
    //max is set to slightly above 0 because there exists some error due to floating point rounding. 
    //So sometimes a distance will be around 10^-15 when it should be 0.
    double max = 0.00000000001;
    double maxIndex = -1;
    //goes through every value in vec and calculates its distance to the line
    for(int i = 0; i < vec.size(); i++){
        double temp = findDistance(v1,v2,vec[i]);
        if(temp > max){
            max = temp;
            maxIndex = i;
        }
    }
    //If no new max is found that means that every point in vector is already on the line created by v1 and v2 and 
    //thus should not be added to the set of points for the convex hull.
    if(max <= 0.00000000001){
        return -1;
    }

    return maxIndex;
}

bool isOutside(Point v1, Point v2, Point p){
    //d will either be positive or negative depending on which side of the line Point p is.
    double d = ((p.x - v1.x) * (v2.y - v1.y)) - ((p.y - v1.y) * (v2.x - v1.x));

    return (d < 0);
}

//rearranges a vector so that the first value in it will be the one with the smallest y value.
void rearrangeVector(vector<Point>& vec){
    double min = DBL_MAX;
    int minI = 0;
    for(int i = 0; i < vec.size();i++){
        if(vec[i].y < min){
            minI = i;
            min = vec[i].y;
        }else if(vec[i].y == min){
            if(vec[i].x < vec[minI].x){
                minI = i;
            }
        }
    }
    Point temp;
    temp.x = vec[minI].x;
    temp.y = vec[minI].y;
    while(vec[0].x != temp.x && vec[0].y != temp.y){
        vec.push_back(vec[0]);
        vec.erase(vec.begin());
    }
}

//finds a point in a vector and returns its index.
//returns -1 if the point isn't in the vector.
int findPoint(vector<Point>& vec, Point p){
    for(int i = 0; i < vec.size(); i++){
        if(vec[i].x == p.x && vec[i].y == p.y){
            return i;
        }
    }
    return -1;
}

//quickhull method which will fill edges with points in vec which will be on the convex hull.
void quickHullLine(vector<Point>& vec, vector<Point>& edges, Point v1, Point v2){
    //exit out if vec doesn't have any points
    if(vec.size() ==0){
        return;
    }
    //pick the furthest point in vec to add
    double maxI = pickPoint(vec,v1,v2);
    //If no good points in vec exit out
    if(maxI == -1){
        return;
    }
    Point newPoint;
    newPoint.x = vec[maxI].x;
    newPoint.y = vec[maxI].y;
    //cout << newPoint.x << "," << newPoint.y  << endl;
    vector<Point>::iterator it;
    it = find(edges.begin(), edges.end(),v1);
    //insert the point in edges to be in between the two points who's line found the new edge.
    //does this to maintain clockwise order for later.
    edges.insert(it+1,newPoint);
    vector<Point> vec2;
    vector<Point> vec3;
    //Adds all points outside to new vectors for later calls of the function.
    //does this to remove any extra points which can't be on the convex hull.
    //Also if points not outside the given line are added to vec in the function call
    //The findDistance function may find a point on the wrong side of the hull.
    for(int i = 0; i < vec.size(); i++){
        //specifically orders the two points in the line in clockwise order for it to correctly pick 
        //from the outside.
        if(isOutside(v1,newPoint,vec[i])){
            vec2.push_back(vec[i]);
        }
        if(isOutside(newPoint,v2, vec[i])){
            vec3.push_back(vec[i]);
        }
    }

    quickHullLine(vec2, edges,v1, newPoint);
    quickHullLine(vec3, edges,newPoint, v2);

}




void quickHull(vector<Point>& vec, vector<Point>& edges, Point v1, Point v2){
    //Breaks the points into two halves. Those above the line created by v1 and v2 and those below.
    vector<Point> outside;
    vector<Point> inside;
    for(int i = 0; i < vec.size();i++){
        if(isOutside(v1,v2,vec[i])){
            outside.push_back(vec[i]);
        }else{
            inside.push_back(vec[i]);
        }
    }
    quickHullLine(outside,edges,v1,v2);
    quickHullLine(inside,edges,v2,v1);

    //edges was in clockwise order so reverse to get into counterclockwise
    reverse(edges.begin(),edges.end());
    removeExtraPoints(edges);
    rearrangeVector(edges);
    //If there are less than two edges the input can't make a convex hull
    if(edges.size() <= 2){
        cout << "invalid input" << endl;
        exit(1);
    }

    //Writing the output as specified by the instructions.
    vector<pair<int,Point> > ans;
    for(int i = 0; i < edges.size();i++){
        int num = findPoint(vec,edges[i]);
        pair<int,Point> p;
        p = make_pair(num,edges[i]);
        ans.push_back(p);
    }
    cout << ans.size() << endl;
    for(int i = 0 ; i < ans.size();i++){
        cout << ans[i].first << ", " << ans[i].second.x << ", " << ans[i].second.y << endl;
    }
}

int main(){
    vector<Point > vec;
    ifstream f;
    string temp;
    int numPoint = 0;
    //cins to get the points
    cin >> numPoint;
    for(int i = 0; i < numPoint; i++){
           char comma;
           Point p;
           cin >> p.x >> comma >> p.y;
           vec.push_back(p);
    }
    //if there are only two points there can't be a convex hull.
    if(vec.size() <= 2){
        cout << "invalid input" << endl;
        exit(1);
    }
    //Find the points with the smallest and largest x values
    // to create the orignal two points in the hull
    double minX = DBL_MAX;
    int minXIndex = 0;
    double maxX = DBL_MIN;
    int maxXIndex = 0;

    for(int i = 0; i < vec.size(); i++){
        if(vec[i].x < minX){
            minX = vec[i].x;
            minXIndex = i;
        }else if(vec[i].x == minX){
            if(vec[i].y > vec[minXIndex].y){
                minX = vec[i].x;
                minXIndex = i;
            }
        }

        if(vec[i].x > maxX){
            maxX = vec[i].x;
            maxXIndex = i;
        }else if(vec[i].x == maxX){
            if(vec[i].y > vec[maxXIndex].y){
                maxX = vec[i].x;
                maxXIndex = i;
            }
        }
    }

    vector<Point> t;
    Point t1;

    t1.x = vec[minXIndex].x;
    t1.y = vec[minXIndex].y;
    t.push_back(t1);
    
    Point t2;
    t2.x = (vec[maxXIndex].x);
    t2.y = (vec[maxXIndex].y);
    t.push_back(t2);

    if(t.size() != 2){
        cout << "size error" << endl;
        exit(1);
    }

    //calls method
    //surronding code is the way I tested the time of the function.
    //found this code online.
    //auto begin = std::chrono::high_resolution_clock::now();

    quickHull(vec,t,t1,t2);

    // auto end = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    // printf("Time measured: %.7f seconds.\n", elapsed.count() * 1e-9);
    return 0;
}