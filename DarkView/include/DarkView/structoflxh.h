#ifndef STRUCTOFLXH_H
#define STRUCTOFLXH_H


typedef struct point {
 float x,y;
}point;

typedef struct pointlist{
 point *p;
 int length;
 int listsize;
}pointlist;

typedef struct square {
 point p1,p2,p3,p4;
}square;

typedef struct circle {
 point center;
 float radius;
 float density;
}circle;


#endif // STRUCTOFLXH_H
