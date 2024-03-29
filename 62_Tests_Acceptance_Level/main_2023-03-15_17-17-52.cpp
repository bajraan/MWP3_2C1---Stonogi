#include <iostream>
#include <algorithm>
#include <math.h>
#include <sstream>


#define MODE_DEB     0
#define MODE_TEST    0
#define PRINT_RAPORT 0



#if PRINT_RAPORT == 1
#include <fstream>
#include "printraport/printraport.h"
#endif // PRINT_RAPORT


using namespace std;


enum direction
{
    neg,    // Negative direction
    pos     // Positive direction
};

enum collision
{
    collisionNO,
    collisionYES
};

enum TimesOverleap
{
    overleapExist_NO,
    overleapExist_YES
};

enum BelongsToInitial
{
    CrossPointBelongsToInitial_FALSE,
    CrossPointBelongsToInitial__TRUE
};

enum belongstoPlayZone
{
    CrossPointBelongsToPlayZone_FALSE,
    CrossPointBelongsToPlayZone__TRUE
};

struct Point
{
    double x;
    double y;
};

struct LINFAC
{
    double A;
    double B;
    double C;
};

struct MILLPEDE
{
    Point   P1;
    Point   P2;
    double  speed;
    LINFAC  lin;
    Point   CrossPoint;
    int     CPdirection;
    int     CPbelongsToInitial;
    double  CrossTimeStart;
    double  CrossTimeEnd;
    int     TimesOverleap;
    int     horizontalTransformed;
};

struct LINCOL
{
    int     Collision;
    double  x;
};


struct PLZ
{
    Point    PL1;
    Point    PL2;
    Point    PL3;
    Point    PL4;

    int Min;
    int Max;

}PlayZone

    {
        -100000, 100000,      100000, 100000,
        -100000,-100000,      100000,-100000,
        -100000,
         100000
    };

struct Shift_M
{
    double Tx;
    double Ty;
};                  // Shift_M[KOLUMNY ][WIERSZE]

struct Rotate_M
{
    double theta;
    double T[2][2]; 
                    // T[1][1]    cos()  
                    // T[1][2]   -sin()
                    // T[2][1]    sin()
                    // T[2][2]    cos()
                    // T[KOLUMNY][WIERSZE]
};


int  CHECK_MillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2);
void DETERMINE_LineEquation(MILLPEDE *ST);
void DETERMINE_CrossPoint(MILLPEDE *T1, MILLPEDE *T2);
void DETERMINE_CrossDirection(MILLPEDE *T1);
void DETERMINE_CrossTimes(MILLPEDE *T1);
int  DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2);
void DETERMINE_ShiftMatrix (MILLPEDE*T, Shift_M*sT);
void DETERMINE_RotateMatrix(MILLPEDE*T,Rotate_M*rT, Rotate_M* rT_Reet);
void DETERMINE_LineCollisions(MILLPEDE *T1, MILLPEDE *T2,LINCOL*tab);
void DETERMINE_SpeedSignCorrection(MILLPEDE *T);
int  CHECK_IF_CollisionPointbelongstoPlayZone(Point P, PLZ PlayZone);
int  CHECK_IF_CrossPointBelongsToMillipedeForInitial(MILLPEDE *T1);
void TRANSFORM_MillpedeToHorizontalMillpede(MILLPEDE*vT, Shift_M sT, Rotate_M rT, MILLPEDE*hT );
void TRANSFORM_PointToOriginal(MILLPEDE *ST1, Shift_M sT, Rotate_M rT, Point P);
void MULL_Matrices2x1and2x2(Point* P, Rotate_M * T, Point* P_pri );



int TEST__CheckMillipedeCollision(void);
int TEST__DETERMINE_LineEquation(void);
int TEST__DETERMINE_CrossPoint(void);
int TEST__DETERMINE_CrossDirection(void);
int TEST__DETERMINE_CrossTimes(void);
int TEST__DETERMINE_Overleap(void);
int TEST__DETERMINE_ShiftMatrix(void);
int TEST__DETERMINE_RotateMatrix(void);
int TEST__DETERMINE_LineCollisions(void);
int TEST__CHECK_IF_CollisionPointbelongstoPlayZone(void);
int TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial(void);
int TEST__TRANSFORM_MillpedeToHorizontalMillpede(void);



void LoadData(MILLPEDE *ST1, MILLPEDE *ST2);
void TerminalSettings(void);
void TestScenarioSettings(void);
void gotox(short);



int main()
{
    #if MODE_TEST == 1
    TerminalSettings();
    TestScenarioSettings();
    #endif


    MILLPEDE ST1;
    MILLPEDE ST2;


    int Cases;
    cin >> Cases;
    while(Cases)
    {
        LoadData(&ST1, &ST2);
        int ret = CHECK_MillipedeCollision(&ST1, &ST2);
        if(ret == collisionYES) cout << "TAK" << endl;
        if(ret == collisionNO) cout << "NIE" << endl;
        Cases--;
    }

    return 0;
}




int CHECK_MillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2)
{

    DETERMINE_LineEquation(ST1); // Determine line eqution for ST!
    DETERMINE_LineEquation(ST2); // Determine line eqution for ST2

    if
    (//==========================================
     ST1->lin.A == ST2->lin.A  //              //
     &&                        //   INFINITE   //
     ST1->lin.B == ST2->lin.B  // INTERSECTION //
     &&                        //              //
     ST1->lin.C == ST2->lin.C  //              //
    )//==========================================
    {

        MILLPEDE    h_ST1;
        MILLPEDE    h_ST2;
        Shift_M     sT;
        Rotate_M    rT;
        Rotate_M    rT_Reet;

        if
        (//======================================================================
          0 == ST1->lin.A       //  HORIZONTAL TRAJECTORY
            &&                  //    ->COPY ORGINAL PARAM 
          1 == ST1->lin.B       //    ->Set horizontalTransformed flag to 0; 
        )//======================================================================
        {
            h_ST1.horizontalTransformed = 0;
            h_ST2.horizontalTransformed = 0;   

            h_ST1.P1 = ST1->P1;
            h_ST1.P2 = ST1->P2;
            h_ST2.P1 = ST2->P1;
            h_ST2.P2 = ST2->P2;
            h_ST1.speed = ST1->speed;
            h_ST2.speed = ST2->speed;
        }
        else
        //=======================================================================
                                //  Different than HORIZONTAL TRAJECTORY
                                //    -> Transform PARAM to horizontal trajectory
                                //    ->Set horizontalTransformed flag to 1; 
        //=======================================================================
        {
            h_ST1.horizontalTransformed = 1;
            h_ST2.horizontalTransformed = 1;   

            DETERMINE_ShiftMatrix (ST1, &sT);
            DETERMINE_RotateMatrix(ST1, &rT, &rT_Reet);

            TRANSFORM_MillpedeToHorizontalMillpede
                ( 
                   ST1, 
                   sT, 
                   rT, 
                  &h_ST1
                );
            
            TRANSFORM_MillpedeToHorizontalMillpede
                ( 
                   ST2, 
                   sT, 
                   rT, 
                  &h_ST2
                );

            h_ST1.speed = ST1->speed;
            h_ST2.speed = ST2->speed;
        }

        LINCOL  Lincoltable[4];

        DETERMINE_LineCollisions(&h_ST1, &h_ST2, Lincoltable);

        for(int i=0; i<4; i++)
        {
            if(Lincoltable[i].Collision == collisionYES )
            {
                if
                ( 
                1 == h_ST1.horizontalTransformed 
                ) 
                {
                    Point h_collisionPoint = {Lincoltable[i].x,0};
                    TRANSFORM_PointToOriginal(ST1, sT, rT_Reet, h_collisionPoint);
                }

                if
                (
                CHECK_IF_CollisionPointbelongstoPlayZone(ST1->CrossPoint, PlayZone)
                ==
                CrossPointBelongsToPlayZone__TRUE
                )
                {
                    return collisionYES;
                }
            }
        }return collisionNO;
    }
    else if
    (//========================================
    ST1->lin.A == ST2->lin.A //              // 
    &&                       //   LACK OF    //         
    ST1->lin.B == ST2->lin.B // INTERSECTION // 
    &&                       //              //     
    ST1->lin.C != ST2->lin.C //              //       
    )//========================================
    {
        return collisionNO;
    }
    else
    //=========================================
                             //              //
                             //     ONE      //
                             // INTERSECTION //
                             //              //
                             //              //    
    //=========================================
    {

        DETERMINE_CrossPoint(ST1,ST2);


        if
        (
        CrossPointBelongsToPlayZone_FALSE 
        ==
        CHECK_IF_CollisionPointbelongstoPlayZone(ST1->CrossPoint, PlayZone)
        )
        {
            return collisionNO; 
        }


        if
        (
        CrossPointBelongsToInitial_FALSE
        ==
        CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST1)
        )
        {
            DETERMINE_CrossDirection(ST1);
            if(neg == ST1->CPdirection ) return collisionNO;
        }

        
        if
        (
        CrossPointBelongsToInitial_FALSE
        ==
        CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST2)
        )
        {
            DETERMINE_CrossDirection(ST2);
            if(neg == ST2->CPdirection ) return collisionNO;
        }


        DETERMINE_CrossTimes(ST1);
        DETERMINE_CrossTimes(ST2);

        
        if
        (
        overleapExist_YES 
        == 
        DETERMINE_Overleap(ST1,ST2)
        )
        {
            return collisionYES;
        }
        else
        {
            return collisionNO;
        }
    }
}


void LoadData(MILLPEDE *ST1, MILLPEDE *ST2)
{
    cin >> ST1->P1.x
        >> ST1->P1.y
        >> ST1->P2.x
        >> ST1->P2.y
        >> ST1->speed;

    cin >> ST2->P1.x
        >> ST2->P1.y
        >> ST2->P2.x
        >> ST2->P2.y
        >> ST2->speed;
}



void DETERMINE_LineEquation(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                     "  << endl;
    cout << "DetermineLineEquation";
    #endif // MODE_DEB

    if
    (//==============================================
                        //  
    T->P1.x == T->P2.x  //  VERTICAL TRAJECTORY
                        //
    )//==============================================
    {
        T->lin.A =  1;
        T->lin.B =  0;
        T->lin.C = -T->P1.x;
    }
    else if
    (//==============================================
                        // 
    T->P1.y == T->P2.y  // HORIZONTAL TRAJECTORY
                        //
    )//==============================================  
    {
        T->lin.A = 0;
        T->lin.B = 1;
        T->lin.C = -T->P1.y;
    }
    else
    //==============================================
                        // 
                        //  ANGULAR TRAJECTORY
                        //
    //==============================================  
    {
        double Slope;
        double Intercept;

            //==============//
            //              //
            //  SLOPE       //  A
            //              //
        if(0 == (max(T->P1.y, T->P2.y) - min(T->P1.y, T->P2.y))) Slope = 0;
        else Slope = (T->P2.y - T->P1.y) / (T->P2.x - T->P1.x);
        T->lin.A = Slope;   //
            //              //
            //              //
            //  INTERCEPT   //  C
            //              //
        if(0 == Slope) Intercept = T->P1.x;
        else Intercept = T->P1.y - Slope * T->P1.x;
        T->lin.C = Intercept;
            //              //
            //              //
            //              //  B
            //              //
        T->lin.B = -1;      //
            //              //
            //              //
            //==============//
    }

    #if MODE_DEB == 1
    cout << " >> (A,B,C): " <<
    "("  << T->lin.A <<
    ","  << T->lin.B <<
    ","  << T->lin.C <<
    ")"  << endl;
    #endif // MODE_DEB
}



//
//
//
// https://www.geeksforgeeks.org/point-of-intersection-of-two-lines-formula/
//
//
//
void DETERMINE_CrossPoint(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "                   " << endl;
    cout << "DetermineCrossPoint";
    #endif // MODE_DEB

    Point CP;

    // TODO: DIVIDING BY ZERO
    // Dividing by zero exception is not nedded, due to math theory. // PBA comment
    // if lines are not parraler they always have intersection.      // PBA comment

    CP.x =
        (T1->lin.B * T2->lin.C - T2->lin.B * T1->lin.C)
                               /
        (T1->lin.A *T2->lin.B  - T2->lin.A * T1->lin.B );

    CP.y =
        (T1->lin.C * T2->lin.A - T2->lin.C * T1->lin.A)
                               /
        (T1->lin.A *T2->lin.B  - T2->lin.A * T1->lin.B );


    T1->CrossPoint.x = CP.x;
    T1->CrossPoint.y = CP.y;
    T2->CrossPoint.x = CP.x;
    T2->CrossPoint.y = CP.y;


    #if MODE_DEB == 1
    cout << " >>  CP: (" << T1->CrossPoint.x <<","<<T1->CrossPoint.y <<")" << endl << endl;
    #endif // MODE_DEB
}



/// @brief 
/// @param T 
/// @param PlayZone 
/// @return 
int CHECK_IF_CollisionPointbelongstoPlayZone(Point P, PLZ PlayZone)
{

    #if MODE_DEB == 1
    cout << "                   " << endl;
    cout << "CHECK_IF_CollisionPointbelongstoPlayZone" << endl;
    #endif // MODE_DEB


    if  
    ( 
    PlayZone.Min <= P.x
    &&
    PlayZone.Max >= P.x
    &&
    PlayZone.Min <= P.y
    &&
    PlayZone.Max >= P.y
    )
         return CrossPointBelongsToPlayZone__TRUE;
    else return CrossPointBelongsToPlayZone_FALSE;
}



int CHECK_IF_CrossPointBelongsToMillipedeForInitial(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "CHECK_IF_CrossPointBelongsToMillipedeForInitial";
    #endif // MODE_DEB


    if
    (
    max(T->P1.x,T->P2.x) >= T->CrossPoint.x && T->CrossPoint.x >= min(T->P1.x,T->P2.x)
    &&
    max(T->P1.y,T->P2.y) >= T->CrossPoint.y && T->CrossPoint.y >= min(T->P1.y,T->P2.y)
    )
         return T->CPbelongsToInitial = CrossPointBelongsToInitial__TRUE;
    else return T->CPbelongsToInitial = CrossPointBelongsToInitial_FALSE;
}



// trzeba policzyc ktory z punktow P1,P2 jest blizej punktu CP,
// je�eli punkt P1 jest bli�ej CP to kierunek jset pozytywny
// je�eli punkt P2 jest bli�ej CP to kierunek jest negatywny
//
// Examples:
// P1<------P2      CP      // DIRECTION NEGATIVE
// P2------>P1      CP      // DIRECTION POSITIVE
// CP    P1<--------P2      // DIRECTION POSITIVE
// CP    P2-------->P1      // DIRECTION NEGATIVE

void DETERMINE_CrossDirection(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                       " << endl;
    cout << "DetermineCrossDirection";
    #endif // MODE_DEB

    double cpp1,cpp2;
    cpp1 = sqrt(pow(T->P1.x - T->CrossPoint.x,2) + pow(T->P1.y - T->CrossPoint.y,2));
    cpp2 = sqrt(pow(T->P2.x - T->CrossPoint.x,2) + pow(T->P2.y - T->CrossPoint.y,2));

    if      (cpp1 <  cpp2) T->CPdirection = pos;
    else if (cpp1 >  cpp2) T->CPdirection = neg;
    else
    {
        #if          MODE_TEST == 1
        FATAL_ERROR__DetermineCrossDirection;  
        #endif    
    }


    #if MODE_DEB == 1
    cout << " >> Direction: ";
    if      (pos == T->CPdirection) cout << "positive"       << endl;
    else if (neg == T->CPdirection) cout << "negative"       << endl;
    else                           cout << "FATAL ERRor"    << endl;
    #endif // MODE_DEB
}



void DETERMINE_CrossTimes(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                   "  << endl;
    cout << "DetermineCrossTimes"  << endl;
    #endif // MODE_DEB

    // t=s/v
    double S1;
    double S2;

    #if MODE_DEB == 1
    //cout << "DEBOUG DATA: " << endl;
    //cout << "P1:   (" << T->P1.x         <<","<< T->P1.y         << ")" << endl;
    //cout << "P2:   (" << T->P2.x         <<","<< T->P2.y         << ")" << endl;
    //cout << "CP:   (" << T->CrossPoint.x <<","<< T->CrossPoint.y << ")" << endl;
    //cout << "Speed: " << T->speed << endl;
    #endif // MODE_DEB

    S1  = 

    sqrt(
            pow(T->P1.x - T->CrossPoint.x ,2)
            +
            pow(T->P1.y - T->CrossPoint.y ,2)
        );

    S2  =

    sqrt(
           pow(T->P2.x - T->CrossPoint.x ,2)
           +
           pow(T->P2.y - T->CrossPoint.y ,2)
        );

    if(CrossPointBelongsToInitial__TRUE == T->CPbelongsToInitial)
    {
        T->CrossTimeStart = 0;
        T->CrossTimeEnd   = S2/T->speed;
    }
    else if(S1<S2)
    {
        T->CrossTimeStart = S1/T->speed;
        T->CrossTimeEnd   = S2/T->speed;
    }
    else if(S1>S2)                          // PBA comment
    {                                       // Case will never occur
        T->CrossTimeStart = S2/T->speed;    //
        T->CrossTimeEnd   = S1/T->speed;    //
    }                                       //
    else
    {                                       // PBA comment
        #if          MODE_TEST == 1         // Case occurs only if 
        FATAL_ERROR__DetermineCrossTimes;   // CP belongs to inital
        #endif                              //
    }                                       //





    #if MODE_DEB == 1
    cout << "-----------------------------"  << endl;
    cout << "P1: (" << T->P1.x << "," << T->P1.y << ")" << endl;
    cout << "P2: (" << T->P2.x << "," << T->P2.y << ")" << endl;
    cout << "S1: " << S1 << endl;
    cout << "S2: " << S2 << endl;
    cout << endl;
    cout.precision(15);
    cout << "CrossTimeStart:          >> "  << T->CrossTimeStart << endl;
    cout << "CrossTimeEnd:            >> "  << T->CrossTimeEnd << endl;
    cout << "-----------------------------"  << endl;
    cout << endl;




    #endif // MODE_DEB
}



int DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DetermineOverleap";
    #endif // MODE_DEB


    if(fabs(T2->CrossTimeStart - T1->CrossTimeStart) < 1e-10)
    {
        T1->TimesOverleap = overleapExist_YES;
        T2->TimesOverleap = overleapExist_YES;
    }
    else if(T1->CrossTimeStart < T2->CrossTimeStart)
    {
        if(fabs(T1->CrossTimeEnd - T2->CrossTimeStart) < 1e-10)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }
        else if(T1->CrossTimeEnd < T2->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_NO;
            T2->TimesOverleap = overleapExist_NO;
        }
        else if (T1->CrossTimeEnd > T2->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }
        else
        {
            #if     MODE_TEST == 1
            FATAL_ERROR__DetermineOverleap;
            #endif
        }
    }
    else if(T2->CrossTimeStart < T1->CrossTimeStart)
    {
        if(fabs(T2->CrossTimeEnd - T1->CrossTimeStart)<1e-10)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;           
        }
        else if (T2->CrossTimeEnd < T1->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_NO;
            T2->TimesOverleap = overleapExist_NO;
        }
        else if(T2->CrossTimeEnd > T1->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }
        else
        {
        #if     MODE_TEST == 1
        FATAL_ERROR__DetermineOverleap;
        #endif
        }
    }
    else
    {
        #if     MODE_TEST == 1
        FATAL_ERROR__DetermineOverleap;
        #endif
    }


    #if MODE_DEB == 1
    if(overleapExist_YES == T1->TimesOverleap)
        cout << " >> overleap_YES" << endl;
    if(overleapExist_NO == T1->TimesOverleap)
        cout << " >> overleap_NO" << endl;
        cout << endl;
    #endif // MODE_DEB

    return T1->TimesOverleap;

}



void DETERMINE_ShiftMatrix(MILLPEDE*T, Shift_M *sT)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DETERMINE_ShiftMatrix";
    #endif // MODE_DEB


    if
    (//===============================================//  |
      1 == T->lin.A         //                        //  |    
      &&                    //  VERTICAL TRAJECTORY   //  |    
      0 == T->lin.B         //                        //  |    
    )//===============================================// ---------------
    {
        sT->Tx = T->lin.C;
        sT->Ty = 0;

    }
    else if
    (//===============================================//           /
                            //                        //          /
      0 > T->lin.A          //  ANGULAR TRAJECTORY    //         /
                            //  A<0                   //        /
    )//===============================================// ---------------
    {
        sT->Tx = -(T->lin.C/T->lin.A);
        sT->Ty = 0;
    }
    else
     //===============================================//   \ 
                            //                        //    \   
                            //  ANGULAR TRAJECTORY    //     \  
                            //  A>0                   //      \ 
     //===============================================// ---------------
    {
        sT->Tx = T->lin.C;
        sT->Ty = 0;
    }
}



void DETERMINE_RotateMatrix(MILLPEDE*T,Rotate_M *rT, Rotate_M *rT_Reet)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DETERMINE_RotateMatrix";
    #endif // MODE_DEB



    if
    (//=========================================//  |
      1 == T->lin.A     //                      //  |    |        
      &&                // VERTICAL Trajectory  //  |    |        
      0 == T->lin.B     //  theta = 90          //  |    |        
    )//=========================================// ---------------
    {

        rT->theta = M_PI/2;

    }
    else if
    (//==========================================//            /     
                        // Angular Trajectory    //       |   /      
      0 <= T->lin.A     // Angular Trajectory    //       |  /                    
                        // 0deg < theta < 90deg  //       | /  
    )//==========================================// --------------- 
    {

         rT->theta = atan(T->lin.A);

    }
    else                                        //  ?
    //==========================================//   ?   |          
                    // Angular Trajectory       //    ?  |         
                    //  90deg < theta < 18deg   //     ? |         
    //==========================================// ---------------
    {

        rT->theta = M_PI + atan(T->lin.A);

    }

    // Return MATRIX  [ Theta > 0 ]
    //      this matrix allows you to return to the original position.
    //
    rT_Reet->T[0][0] = cos(rT->theta); rT_Reet->T[1][0] = -sin(rT->theta);
    rT_Reet->T[0][1] = sin(rT->theta); rT_Reet->T[1][1] =  cos(rT->theta);
    //   [kolumny][wiersze]


    rT->theta *= -1;  // Change ratate direction 

    // Return MATRIX  [ Theta < 0 ]
    //      this matrix allows you Transform point.
    //
    rT->T[0][0] = cos(rT->theta); rT->T[1][0] = -sin(rT->theta);
    rT->T[0][1] = sin(rT->theta); rT->T[1][1] =  cos(rT->theta);
    //   [kolumny][wiersze]

}



void MULL_Matrices2x1and2x2(Point* P, Rotate_M * T, Point* P_pri )
{
    //      [KOLUMNY][WIERSZE]
    P_pri->x = T->T[0][0] * P->x + T->T[1][0] * P->y;
    P_pri->y = T->T[0][1] * P->x + T->T[1][1] * P->y;
}



void TRANSFORM_MillpedeToHorizontalMillpede(MILLPEDE*MILL, Shift_M sT, Rotate_M rT, MILLPEDE*hT )
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "TRANSFORM_MillpedeToHorizontalMillpede";
    #endif // MODE_DEB

    hT->P1.x = 0;
    hT->P1.y = 0;
    hT->P2.x = 0;
    hT->P2.y = 0;


    if(MILL->lin.A >= 0)
    {
    hT->P1.x = MILL->P1.x + sT.Tx;
    hT->P1.y = MILL->P1.y + sT.Ty;
    hT->P2.x = MILL->P2.x + sT.Tx;
    hT->P2.y = MILL->P2.y + sT.Ty;
    }


    if(MILL->lin.A < 0)
    {
    hT->P1.x = MILL->P1.x - sT.Tx;
    hT->P1.y = MILL->P1.y - sT.Ty;
    hT->P2.x = MILL->P2.x - sT.Tx;
    hT->P2.y = MILL->P2.y - sT.Ty;
    }


    #if     MODE_DEB == 1
    cout << endl;
    cout << endl;
    cout << "MILL P1: " << MILL->P1.x << "," << MILL->P1.y << endl;
    cout << "MILL P2: " << MILL->P2.x << "," << MILL->P2.y << endl;
    cout << "Shifted P1: " << hT->P1.x << "," << hT->P1.y << endl;
    cout << "Shifted P2: " << hT->P2.x << "," << hT->P2.y << endl;
    #endif


    Point temp;


    MULL_Matrices2x1and2x2(&hT->P1, &rT, &temp );
    hT->P1 = temp;

    MULL_Matrices2x1and2x2(&hT->P2, &rT, &temp );
    hT->P2 = temp;


    #if     MODE_DEB == 1
    cout << "Rotated P1: " << hT->P1.x << "," << hT->P1.y << endl;
    cout << "Rotated P2: " << hT->P2.x << "," << hT->P2.y << endl;
    cout << endl;
    cout << endl;
    #endif
}



void DETERMINE_LineCollisions(MILLPEDE *T1, MILLPEDE *T2, LINCOL* tab)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DETERMINE_LineCollisions" << endl;;
    #endif // MODE_DEB

    double t;

    DETERMINE_SpeedSignCorrection(T1);
    DETERMINE_SpeedSignCorrection(T2);

    if( 0 == T1->speed - T2->speed )
    {
        tab[0].Collision = collisionNO;
        tab[1].Collision = collisionNO;
        tab[2].Collision = collisionNO;
        tab[3].Collision = collisionNO;
    }
    else
    {     
        tab[0].Collision = collisionYES;
        tab[1].Collision = collisionYES;
        tab[2].Collision = collisionYES;
        tab[3].Collision = collisionYES;
        
//==============================================================
// (1) T1P1(t) = T1P1 + V1*t
// (2) T2P1(t) = T2P1 + V2*t
//
// Collision Condisiotn: T1P1(t) = T2P1(t)
//
// T1P1 + V1*t = T2P1 + V2*t
// T1P1 - T2P1 = V2*t - V1*t
// T1P1 - T2P1 = t(V2 - V1)
//
// t = (T1P1 - T2P1) / (V2 - V1)
//==============================================================
        t = (T1->P1.x - T2->P1.x)/(T2->speed - T1->speed);
        if(t<0)     tab[0].Collision = collisionNO;
        else        tab[0].x = T1->P1.x + T1->speed * t; 
// cout << "=========================================" << endl;
// cout << "T1: " << t << endl;
// cout << "=========================================" << endl;

//==============================================================
// (1) T1P1(t) = T1P1 + V1*t
// (2) T2P2(t) = T2P2 + V2*t
//
// Collision Condisiotn: T1P1(t) = T2P2(t)
//
// T1P1 + V1*t = T2P2 + V2*t
// T1P1 - T2P2 = V2*t - V1*t
// T1P1 - T2P2 = t(V2 - V1)
//
// t = (T1P1 - T2P2) / (V2 - V1)
//==============================================================
        t = (T1->P1.x - T2->P2.x)/(T2->speed - T1->speed);
        if(t<0)     tab[1].Collision = collisionNO;
        else        tab[1].x = T1->P1.x + T1->speed * t;  
// cout << "=========================================" << endl;
// cout << "T2: " << t << endl;
// cout << "=========================================" << endl;

//==============================================================
// (1) T1P2(t) = T1P2 + V1*t
// (2) T2P1(t) = T2P1 + V2*t
//
// Collision Condisiotn: T1P2(t) = T2P1(t)
//
// T1P2 + V1*t = T2P1 + V2*t
// T1P2 - T2P1 = V2*t - V1*t
// T1P2 - T2P1 = t(V2 - V1)
//
// t = (T1P2 - T2P1) / (V2 - V1)
//==============================================================
        t = (T1->P2.x - T2->P1.x)/(T2->speed - T1->speed);
        if(t<0)     tab[2].Collision = collisionNO;
        else        tab[2].x = T1->P2.x + T1->speed * t; 
// cout << "=========================================" << endl;
// cout << "T3: " << t << endl;
// cout << "=========================================" << endl;

//==============================================================
// (1) T1P2(t) = T1P2 + V1*t
// (2) T2P2(t) = T2P2 + V2*t
//
// Collision Condisiotn: T1P2(t) = T2P2(t)
//
// T1P2 + V1*t = T2P2 + V2*t
// T1P2 - T2P2 = V2*t - V1*t
// T1P2 - T2P2 = t(V2 - V1)
//
// t = (T1P2 - T2P2) / (V2 - V1)
//==============================================================
        t = (T1->P2.x - T2->P2.x)/(T2->speed - T1->speed);
        if(t<0)     tab[3].Collision = collisionNO;
        else        tab[3].x = T1->P2.x + T1->speed * t;
// cout << "=========================================" << endl;
// cout << "T4: " << t << endl;
// cout << "=========================================" << endl;

    }


    #if MODE_DEB == 1
    cout << endl;
    cout << tab[0].Collision << " | CPx: (" << tab[0].x << ")" << endl;
    cout << tab[1].Collision << " | CPx: (" << tab[1].x << ")" << endl;
    cout << tab[2].Collision << " | CPx: (" << tab[2].x << ")" << endl;
    cout << tab[3].Collision << " | CPx: (" << tab[3].x << ")" << endl;
    cout << endl;
    cout << "t1: " << (T1->P1.x - T2->P1.x)/(T2->speed - T1->speed) << endl;
    cout << "t2: " << (T1->P1.x - T2->P2.x)/(T2->speed - T1->speed) << endl;
    cout << "t3: " << (T1->P2.x - T2->P1.x)/(T2->speed - T1->speed) << endl;
    cout << "t4: " << (T1->P2.x - T2->P2.x)/(T2->speed - T1->speed) << endl;
    cout << endl;
    cout << "s1 = " << T1->P1.x << "+" << T1->speed << " * t1" << endl;
    cout << "s2 = " << T1->P1.x << "+" << T1->speed << " * t2" << endl;
    cout << "s3 = " << T1->P2.x << "+" << T1->speed << " * t3" << endl;
    cout << "s4 = " << T1->P2.x << "+" << T1->speed << " * t4" << endl;
    #endif // 

}



void DETERMINE_SpeedSignCorrection(MILLPEDE *T)
{
    if
    (//==========================================================//
                        //                                       //
    T->P2.x < T->P1.x   //       P2      0       P1              //
                        // ------|=======|======>|------------>  //
    )//==========================================================//
    
        T->speed *=  1;
    
    else if 
    (//==========================================================//
                        //                                       //
    T->P1.x < T->P2.x   //       P1      0       P2              //
                        // ------|<======|=======|------------>  //
    )//==========================================================//
       
        T->speed *= -1;

    else
    {
        #if     MODE_TEST == 1
        FATAL_ERROR__DetermineSpeedSignCorrection
        #endif
    }
}



void TRANSFORM_PointToOriginal(MILLPEDE *MILL, Shift_M sT, Rotate_M rT, Point P)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "TRANSFORM_PointToOriginal";
    #endif // MODE_DEB


    MILL->CrossPoint.x = P.x;
    MILL->CrossPoint.y = P.y;


    Point temp;
    MULL_Matrices2x1and2x2(&MILL->CrossPoint, &rT, &temp );
    MILL->CrossPoint = temp;


    #if     MODE_DEB == 1
    cout << endl;
    cout << " INPUT  P: " << P.x << "," << P.y << endl;
    cout << "Rotated P: " << MILL->CrossPoint.x << "," << MILL->CrossPoint.y << ")" << endl;
    #endif


    if(MILL->lin.A >= 0)
    {
    MILL->CrossPoint.x = MILL->CrossPoint.x - sT.Tx;
    MILL->CrossPoint.y = MILL->CrossPoint.y - sT.Ty;
    }


    if(MILL->lin.A < 0)
    {
    MILL->CrossPoint.x = MILL->CrossPoint.x + sT.Tx;
    MILL->CrossPoint.y = MILL->CrossPoint.y + sT.Ty;
    }


    #if     MODE_DEB == 1
    cout << "Shifted P: " <<MILL->CrossPoint.x << "," << MILL->CrossPoint.y << endl;
    cout << " OUTPUT P: " <<MILL->CrossPoint.x << "," << MILL->CrossPoint.y << endl;
    #endif

}


