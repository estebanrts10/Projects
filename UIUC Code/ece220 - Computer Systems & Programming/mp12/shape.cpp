/* Our mp12 finds the area and volume of rectangles and
rectangular prisms, and finds the max area and volume
of the ones given. Utilizes a vector to store shapes objects.
Partner: ry6*/




#include "shape.hpp"

//Base class
//Please implement Shape's member functions
//constructor, getName()

Shape::Shape(string name){
    name_=name;
}
string Shape::getName(){
    //TODO
    return name_;
}


//Rectangle
//Please implement the member functions of Rectangle:
//getArea(), getVolume(), operator+, operator-
//copy(), clear()

void Rectangle::copy(const Rectangle& other){
    spec = new double[2];
    spec[0]=other.getWidth();
    spec[1]=other.getLength();
}
void Rectangle::clear(){
    //TODO
    delete []spec;
}
Rectangle::Rectangle(double width, double length):Shape("Rectangle"){
    spec = new double[2];
    spec[0] = width;
    spec[1] = length;
}
Rectangle::Rectangle(const Rectangle& other):Shape("Rectangle"){
    copy(other);
}
Rectangle::~Rectangle(){
    clear();
}
const Rectangle& Rectangle::operator = (const Rectangle& other){
    clear();
    copy(other);
    return *this;
}
double Rectangle::getArea()const{
    //TODO
    return spec[0]*spec[1];
}
double Rectangle::getVolume()const{
    //TODO
    return 0;
}
Rectangle Rectangle::operator + (const Rectangle& rhs){
    //TODO
    double temp = rhs.getWidth()+spec[0];
    double temp1 = rhs.getLength()+spec[1];
    return Rectangle(temp,temp1);
}

Rectangle Rectangle::operator - (const Rectangle& rhs){
    //TODO
    double temp,temp1;
    temp = -rhs.getWidth()+spec[0];
    temp1 = -rhs.getLength()+spec[1];
    if(temp <0){
      temp = 0;
    }
    if(temp1<0){
      temp1=0;
    }
    return Rectangle(temp,temp1);


}

// double * spec;
//spec[0] should be width
//spec[1] should be length
double Rectangle::getWidth()const{return spec? spec[0]:0;}
double Rectangle::getLength()const{return spec? spec[1]:0;}
void Rectangle::setWidth(double width){
    if (spec == NULL)
        return;

    spec[0] = width;
}
void Rectangle::setLength(double length){
    if (spec == NULL)
        return;

    spec[1] = length;
}


//Rectprism
//Please implement the member functions of RectPrism:
//constructor, getArea(), getVolume(), operator+, operator-
//@@Insert your code here
void RectPrism::copy(const RectPrism& other){
    spec = new double[3];
    spec[0] = other.getLength();
    spec[1] = other.getWidth();
    spec[2] = other.getHeight();
}
void RectPrism::clear(){
    //TODO
    delete []spec;
}
RectPrism::RectPrism(double width, double length, double height):Shape("RectPrism"){
    spec = new double[3];
    spec[0] = length;
    spec[1] = width;
    spec[2] = height;
}
RectPrism::RectPrism(const RectPrism& other):Shape("RectPrism"){
    copy(other);
}
RectPrism::~RectPrism(){
    clear();
}
const RectPrism& RectPrism::operator = (const RectPrism& other){
    clear();
    copy(other);
    return *this;
}
double RectPrism::getVolume()const{
    //TODO
    return spec[0]*spec[1]*spec[2];
}
double RectPrism::getArea()const{
    //TODO
    return 2*(spec[0]*spec[1]+spec[0]*spec[2]+spec[1]*spec[2]);
}
RectPrism RectPrism::operator + (const RectPrism& rhs){
    int w,h,l;
    l = rhs.getLength()+spec[0];
    w = rhs.getWidth()+spec[1];
    h = rhs.getHeight()+spec[2];
    return RectPrism(l,w,h);
}

RectPrism RectPrism::operator - (const RectPrism& rhs){
    //TODO
    int w,h,l;
    l = -rhs.getLength()+spec[0];
    w = -rhs.getWidth()+spec[1];
    h = -rhs.getHeight()+spec[2];
    if(l < 0){
      l = 0;
    }
    if(w < 0){
      w = 0;
    }
    if(h < 0){
      h = 0;
    }
    return RectPrism(w,l,h);
}

// double * spec;
//spec[0] should be length
//spec[1] should be width
//spec[2] should be height
double RectPrism::getWidth()const{return spec? spec[1]:0;}
double RectPrism::getHeight()const{return spec? spec[2]:0;}
double RectPrism::getLength()const{return spec? spec[0]:0;}
void RectPrism::setWidth(double width){
    if (spec == NULL)
        return;

    spec[1] = width;
}
void RectPrism::setHeight(double height){
    if (spec == NULL)
        return;

    spec[2] = height;
}
void RectPrism::setLength(double length){
    if (spec == NULL)
        return;

    spec[0] = length;
}



// Read shapes from test.txt and initialize the objects
// Return a vector of pointers that points to the objects
vector<Shape*> CreateShapes(char* file_name){
    //@@Insert your code here

    ifstream ifs (file_name, std::ifstream::in);
    double num_shapes = 0;
    ifs >> num_shapes;
    vector<Shape*> shape_ptrs(num_shapes, NULL);
    //TODO
    string name;
    double w, l, h;
    int i;
    for(i=0;i<num_shapes;i++){


      ifs >> name;
      if(name=="Rectangle"){
        ifs >> w >> l;
        shape_ptrs[i] = new Rectangle(w,l);
      }
      else if (name=="RectPrism"){
        ifs >> w >> l >> h;
        shape_ptrs[i] = new RectPrism(w,l,h);
      }


}
      //create array(vector)



    ifs.close();
    return shape_ptrs;
}

// call getArea() of each object
// return the max area
double MaxArea(vector<Shape*> shapes){
    double maxArea=0;
    int i;
    for(i=0;i<shapes.size();i++){
      if(shapes[i]->getArea()>maxArea){
        maxArea=shapes[i]->getArea();
      }



    }


return maxArea;


}


// call getVolume() of each object
// return the max volume
double MaxVolume(vector<Shape*> shapes){
double maxVolume=0;
int i;
for(i=0;i<shapes.size();i++){
  if(shapes[i]->getVolume()>maxVolume){
    maxVolume=shapes[i]->getVolume();
  }



}


return maxVolume;

}
