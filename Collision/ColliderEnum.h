#ifndef COLLIDERENUM_H
#define COLLIDERENUM_H

//Dictates the type of a collider
typedef enum
{
        COLLIDER_SPHERE = 1,
        COLLIDER_AABB,
        COLLIDER_CONVEXHULL,
	COLLIDER_RAY
} ColliderType;


#endif
