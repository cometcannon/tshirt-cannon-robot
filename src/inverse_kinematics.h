#ifndef inv_kin_h
#define inv_kin_h

#include <Eigen/Dense>
#include <math.h>
using Eigen::MatrixXd;

/* Paper on which these FK's are based: 

http://dx.doi.org/10.4236/ica.2013.42021

The following code assumes that the wheeled robot has the configuration
shown in figure 2 on page 3. Our implementation of the inverse kinematics of this robot
is thus the implementation of equation 6 on the same page. 

*/

//To be measured in meters:
#define b
#define a
#define r   0.1524 //Meters
#define l

MatrixXd ComputeWheelVelocities(MatrixXd Zd)
{
    MatrixXd J; //Jacobian
    J << sqrt(2.0)/2.0,  sqrt(2.0)/2.0,  l * sin(PI/4.0 - atan(b/a)),
         sqrt(2.0)/2.0,  -sqrt(2.0)/2.0, l * sin(PI/4.0 - (PI - atan(b/a))),
         -sqrt(2.0)/2.0, -sqrt(2.0)/2.0, l * sin(PI/4.0 - (PI + atan(b/a))),
         -sqrt(2.0)/2.0, sqrt(2.0)/2.0,  l * sin(PI/4.0 - (2 * PI - atan(b/a)));

    J *= -sqrt(2)/r;

    return J * Zd;
}



#endif