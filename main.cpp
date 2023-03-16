#include <iostream>
#include <algorithm>
#include <math.h>
#include <sstream>


#define MODE_DEB     0
#define MODE_TEST    0
#define PRINT_RAPORT 0


#if MODE_TEST == 1
#include <windows.h>
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#define TEST_PASS   SetConsoleTextAttribute(hConsole,10);cout<<"PASS";SetConsoleTextAttribute(hConsole,7);
#define TEST_FAIL   SetConsoleTextAttribute(hConsole,12);cout<<"FAIL";SetConsoleTextAttribute(hConsole,7);

#define FATAL_ERROR__DetermineCrossTimes SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineCrossTimes()\n";SetConsoleTextAttribute(hConsole,7);
#define FATAL_ERROR__DetermineOverleap   SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineOverleap()\n";SetConsoleTextAttribute(hConsole,7);
#define FATAL_ERROR__DetermineSpeedSignCorrection   SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineSpeedSignCorrection()\n";SetConsoleTextAttribute(hConsole,7);
#define FATAL_ERROR__DetermineCrossDirection SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineCrossDirection()\n";SetConsoleTextAttribute(hConsole,7);
#endif // MODE_TEST

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
                else
                {
                    ST1->CrossPoint = {Lincoltable[i].x,0};
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
// je¾eli punkt P1 jest bli¾ej CP to kierunek jset pozytywny
// je¾eli punkt P2 jest bli¾ej CP to kierunek jest negatywny
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



#if MODE_TEST == 1
//===============================================================
//===============================================================
//
//
//
//      TEST FUNCTIONS
//
//
//
//===============================================================
//===============================================================



void TerminalSettings(void){

    #if MODE_TEST == 1
    cout << "=============================" << endl;
    cout << "         MODE_TEST  =1     " << endl;
    cout << "=============================" << endl;
    #endif // MODE_TEST
    #if MODE_DEB == 1
    cout << "=============================" << endl;
    cout << "         MODE_DEB  =1     " << endl;
    cout << "=============================" << endl;
    #endif // MODE_DEB
}



void TestScenarioSettings()
{
    #if MODE_TEST == 1
    //TEST__DETERMINE_LineEquation();
    //TEST__DETERMINE_CrossPoint();
    //TEST__DETERMINE_CrossDirection();
    //TEST__DETERMINE_CrossTimes();
    //TEST__DETERMINE_Overleap();
    //TEST__DETERMINE_LineCollisions();
    //TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial();
    //TEST__CHECK_IF_CollisionPointbelongstoPlayZone();
    //TEST__DETERMINE_RotateMatrix();
    //TEST__TRANSFORM_MillpedeToHorizontalMillpede();

    TEST__CheckMillipedeCollision();

    #endif // MODE_TEST
}



void gotox(short x)
{ 

    short y;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo (output, &csbi);
    y = csbi.dwCursorPosition.Y;
    COORD pos = {x, y};
    SetConsoleCursorPosition(output, pos);
}



int TEST__DETERMINE_LineEquation( void )
{

    int AmountOfTest = 11;
    struct TESTTable
    {
    Point P1;
    Point P2;
    LINFAC expected;
    };

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine line equation TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    TESTTable TT[AmountOfTest] = {

	// VERTICAL LINE
    //----------------| |-----------------|
    //        |       | | Expected LINFAC |
    //  P1    |  P2   | |  A  |  B  |  C  |
    //        |       | |     |     |     |
          0, 2,   0, 4,      1,    0,    0,
         10, 0,  10,10,      1,    0,  -10,
          0, 4,   0, 2,      1,    0,    0,
          3, 4,   3,-2,      1,    0,   -3,

	// HORIZONTAL LINE
    //-----------------| |-----------------|
    //        |        | | Expected LINFAC |
    //   P1   |   P2   | |  A  |  B  |  C  |
    //        |        | |     |     |     |
        2, 0  ,  4, 0  ,      0,    1,    0,
        2,10  ,  4,10  ,      0,    1,  -10,
        2,-5  ,  4,-5  ,      0,    1,    5,

	// ANGULAR LINE
    //----------------| |-------------------------------|
    //        |       | | Expected LINFAC               |
    //  P1    |  P2   | |      A      | B |      C      |
    //        |       | |             |   |             |
         -1,-2, 1, 2,                2, -1,            0,
         -4,-2,-1,-4,     (double)-2/3, -1,(double)-14/3,
         -1, 2, 9, 0,     (double)-1/5, -1,(double)  9/5,
          1, 1, 2, 2,                1, -1,            0
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL = {0,0,0,0,0,0,0};
        MILL.P1 = TT[i].P1;
        MILL.P2 = TT[i].P2;


        DETERMINE_LineEquation(&MILL);


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
             (fabs(MILL.lin.A - TT[i].expected.A) <= 1e-10)
             &&
             (fabs(MILL.lin.C - TT[i].expected.C) <= 1e-10)
             &&
             (fabs(MILL.lin.B - TT[i].expected.B) <= 1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }



        cout << "  ||  ";
        cout << "P1("<<TT[i].P1.x<<","<<TT[i].P1.y<<")";
        cout << "P2("<<TT[i].P2.x<<","<<TT[i].P2.y<<")";
        cout << "    ";

        gotox(50);
        cout << "(A,B,C): EXP: ("
             << TT[i].expected.A << ","
             << TT[i].expected.B << ","
             << TT[i].expected.C << ")";
        gotox(80);
        cout << "  OBS: ("
             << MILL.lin.A << ","
             << MILL.lin.B << ","
             << MILL.lin.C << ")" << endl;
    }
    return 0;
}



int TEST__DETERMINE_CrossPoint(void)
{

    struct TESTTable
    {
    Point    S1P1;
    Point    S1P2;
    Point    S2P1;
    Point    S2P2;
    Point    Expected_CP;
    };

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine cross point TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    int AmountOfTest = 10;
    TESTTable TT[AmountOfTest] =
    {
    
    //ST1             |ST2            |CP      |
    //P1      |P2     |P1     |P2     |Expect  |
    //        |       |       |       |        |
          0, 2,   0, 4,  2,  0,  4,  0,   0,   0,
         -2, 0,  -4, 0,  0, -2,  0, -4,   0,   0,
         -1, 1,  -2, 2,  1,  1,  2,  2,   0,   0,
         -1, 2,  -2, 3,  1,  2,  2,  3,   0,   1,
          0, 1,  -1, 1,  1,  0,  1, -1,   1,   1,
          1, 2,   0, 2,  2,  1,  2,  0,   2,   2,
          1, 2,  -1, 2,  2,  1,  2, -1,   2,   2,
         -4, 2,  -6, 2, -2,  0, -2, -2,  -2,   2,
         -2,-1,  -2, 1, -1, -2,  1, -2,  -2,  -2,
          0, 0,  -1, 1,  0,  1,  1,  2,-0.5, 0.5,
    };

    for(int i=0; i<AmountOfTest; i++)
    {

        #if MODE_DEB == 1
        cout <<                            endl;
        cout <<                            endl;
        cout << "    ST1(P1)(P2) >> "
             << "(" << TT[i].S1P1.x << "," << TT[i].S1P1.y << ")"
             << "(" << TT[i].S1P2.x << "," << TT[i].S1P2.y << ")" << endl;
        cout << "    ST2(P1)(P2) >> "
             << "(" << TT[i].S2P1.x << "," << TT[i].S2P1.y << ")"
             << "(" << TT[i].S2P2.x << "," << TT[i].S2P2.y << ")" << endl;
        cout << "    ------------------------------" << endl;
        #endif // MODE_DEB


        MILLPEDE MILL1;
        MILLPEDE MILL2;


        MILL1.P1 = TT[i].S1P1;
        MILL1.P2 = TT[i].S1P2;
        MILL2.P1 = TT[i].S2P1;
        MILL2.P2 = TT[i].S2P2;


        DETERMINE_LineEquation(&MILL1);
        DETERMINE_LineEquation(&MILL2);
        DETERMINE_CrossPoint(&MILL1, &MILL2);


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
         (fabs(MILL1.CrossPoint.x - TT[i].Expected_CP.x) <=1e-10)
         &&
         (fabs(MILL1.CrossPoint.y - TT[i].Expected_CP.y) <=1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }


        cout << " || ";
        cout << "exp CP: ("<< TT[i].Expected_CP.x<<","<<TT[i].Expected_CP.y<<")";
        cout << " || ";
        cout << "obs CP: ("<<MILL1.CrossPoint.x  <<","<<MILL1.CrossPoint.y <<")";
        cout << endl;

        #if MODE_DEB == 1
        //cout << "x:      "<<MILL1.CrossPoint.x << TT[i].Expected_CP.x << endl;
        //cout << "logicx: "<<(fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        //cout << "fabs:   "<<fabs(MILL1.CrossPoint.y    - TT[i].Expected_CP.y) << endl;
        //cout << "y:      "<<MILL1.CrossPoint.y << TT[i].Expected_CP.y << endl;
        //cout << "logicx: "<< (fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        #endif // MODE_DEB
    }
    return 0;
}

int TEST__CHECK_IF_CollisionPointbelongstoPlayZone(void)
{


    cout <<                                                          "\n";
    cout << " >>>                                                     \n";
    cout << " >>> TEST FOR: CHECK_IF_CollisionPointbelongstoPlayZone()\n";
    cout << " >>>                                                     \n";
    cout <<                                                          "\n";


    struct TESTTable
    {
    Point CP;
    int expected_result;
    };


    int AmountOfTest = 19;
    TESTTable TT[AmountOfTest] =
    {


    //|--------|----------------------------------|
    //|   CP   |           Expected result        |
    //|        |                                  |
          0,      3, CrossPointBelongsToPlayZone__TRUE,
          3,      0, CrossPointBelongsToPlayZone__TRUE,
          0,     10, CrossPointBelongsToPlayZone__TRUE,
         10,      0, CrossPointBelongsToPlayZone__TRUE,
        100,    100, CrossPointBelongsToPlayZone__TRUE,
          0,     -1, CrossPointBelongsToPlayZone__TRUE,

      99999,  99999, CrossPointBelongsToPlayZone__TRUE,
     -99999, -99999, CrossPointBelongsToPlayZone__TRUE,
      99999, -99999, CrossPointBelongsToPlayZone__TRUE,
    -100000,-100000, CrossPointBelongsToPlayZone__TRUE,
     100000, 100000, CrossPointBelongsToPlayZone__TRUE,
     100000,-100000, CrossPointBelongsToPlayZone__TRUE,

        1,   100001, CrossPointBelongsToPlayZone_FALSE,
        1,   100002, CrossPointBelongsToPlayZone_FALSE,
        0,   999999, CrossPointBelongsToPlayZone_FALSE,
 99999999,   999999, CrossPointBelongsToPlayZone_FALSE,
  -110000,        0, CrossPointBelongsToPlayZone_FALSE,
   110000,        1, CrossPointBelongsToPlayZone_FALSE,
   102000,     -400, CrossPointBelongsToPlayZone_FALSE


    };
    

    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;
        MILL.CrossPoint  = TT[i].CP;


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
         TT[i].expected_result 
         ==
         CHECK_IF_CollisionPointbelongstoPlayZone(MILL.CrossPoint, PlayZone)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }
        

        cout <<"  ||  "
             <<"("<<MILL.CrossPoint.x<<","<<MILL.CrossPoint.y<<") ";
        gotox(30);
        cout <<"  ||   ";
        cout <<"  exp: " << TT[i].expected_result;
        gotox(60);
        cout <<"  obs: " << CHECK_IF_CollisionPointbelongstoPlayZone(MILL.CrossPoint, PlayZone);
        cout << endl;

    }

    return -1;
}



int TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial(void)
{
    struct TESTTable
    {
        Point    P1;
        Point    P2;
        Point    CP;
        int     expected_result;
    };


    cout <<                                                                         endl;
    cout << " >>> "                                                             <<  endl;
    cout << " >>> TEST FOR: CheckIfCrossPointBelongsToMillipedeForInitial() "   <<  endl;
    cout << " >>> "                                                             <<  endl;
    cout <<                                                                         endl;


    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =
    {

    //|--------|--------|--------|----------------------------------|
    //|   P1   |   P2   |   CP   |           Expected result        |
    //|        |        |        |                                  |
          0,  0,   0,  6,   0,  3, CrossPointBelongsToInitial__TRUE ,
          0,  0,   6,  0,   3,  0, CrossPointBelongsToInitial__TRUE ,
          0,  0,   0,  6,   0, 10, CrossPointBelongsToInitial_FALSE ,
          0,  0,   6,  0,  10,  0, CrossPointBelongsToInitial_FALSE ,
          0,  0,   1,  1, 100,100, CrossPointBelongsToInitial_FALSE ,
         -1, -1,   1, -1,   0, -1, CrossPointBelongsToInitial__TRUE
    };

    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;

        MILL.P1          = TT[i].P1;
        MILL.P2          = TT[i].P2;
        MILL.CrossPoint  = TT[i].CP;


        CHECK_IF_CrossPointBelongsToMillipedeForInitial(&MILL);


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if(MILL.CPbelongsToInitial == TT[i].expected_result)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }
        cout << endl;
    }
    return -1;
}



int TEST__DETERMINE_CrossDirection(void)
{
   struct TESTTable
    {
    Point P1;
    Point P2;
    Point CP;
    int expected_direction;
    };


    cout <<                                                 endl;
    cout << " >>> "                                      << endl;
    cout << " >>> TEST FOR: DetermineCrossDirection() "  << endl;
    cout << " >>> "                                      << endl;
    cout <<                                                 endl;


    int AmountOfTest = 10;
    TESTTable TT[AmountOfTest] =
    {
    //|----------|----------|------------------|-----------------|
    //|    P1    |    P2    |        CP        | Expected result |
    //|          |          |                  |                 |
            -1, 2,      9, 0,       10,    -0.2,    neg          ,
            -1, 2,      9, 0,       20,   9/5-4,    neg          ,
            -1, 2,      9, 0,      -20,   9/5+4,    pos          ,
             1, 1,      2, 2,        0,       0,    pos          ,
             1, 1,      2, 2,     -111,    -111,    pos          ,
             1, 1,      2, 2,   -60000,  -60000,    pos          ,
             1, 1,      2, 2,        3,       3,    neg          ,
            -1,-2,      1, 2,        2,       4,    neg          ,
            -1,-2,      1, 2,       -2,      -4,    pos          ,
            -4, 0,     -2,-2,-0.818182,-3.18182,    neg
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;

        MILL.P1.x          = TT[i].P1.x;
        MILL.P1.y          = TT[i].P1.y;
        MILL.P2.x          = TT[i].P2.x;
        MILL.P2.y          = TT[i].P2.y;
        MILL.CrossPoint.x  = TT[i].CP.x;
        MILL.CrossPoint.y  = TT[i].CP.y;

        DETERMINE_CrossDirection(&MILL);

        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";

        if(MILL.CPdirection == TT[i].expected_direction)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout <<"  ||  ";
        cout <<"("<<MILL.P1.x<<","<<MILL.P1.y<<") ";
        cout <<"("<<MILL.P2.x<<","<<MILL.P2.y<<") ";
        cout <<"("<<MILL.CrossPoint.x<<","<<MILL.CrossPoint.y<<") ";
        cout <<"  ||";
        cout <<"  exp: " << TT[i].expected_direction;
        cout <<"  obs: " << MILL.CPdirection;
        cout << endl;

    }
    return -1;
}
int TEST__DETERMINE_CrossTimes(void)
{
    struct TESTTable
    {
    Point   P1;
    Point   P2;
    double  speed;
    Point   CP;
    int     CPbelongsToInitial;
    double  expected_CrossStart;
    double  expected_CrossEnd;
    };


    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =
    {
    //|----------|----------|------------------|--------------|---------------------------------|exp  |exp  |
    //|    P1    |    P2    |        speed     |      CP      |    CPbelongsToInitial result    |cross|cross|
    //|          |          |                  |              |                                 |start|end  |
          1,    0,    0,   0,                 1,        2,   0, CrossPointBelongsToInitial_FALSE,    1,    2,
         -1,   -2,    1,   2,   (double)sqrt(5),        2,   4, CrossPointBelongsToInitial_FALSE,    1,    3,
          4,    3,    4,  -1,                 2,        4,   5, CrossPointBelongsToInitial_FALSE,    1,    3,
          3,    2,    1,   2,                 1,        4,   2, CrossPointBelongsToInitial_FALSE,    1,    3,
         -2,    2,   -6,  -2,   (double)sqrt(8),        0,   4, CrossPointBelongsToInitial_FALSE,    1,    3,
        100,    0,  200,   0,                 1,        0,   0, CrossPointBelongsToInitial_FALSE,  100,  200
    };


    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> TEST FOR: DetermineCrossTimes() " << endl;
    cout << " >>> " << endl;
    cout << endl;


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;
        MILL.P1                 = TT[i].P1;
        MILL.P2                 = TT[i].P2;
        MILL.CrossPoint         = TT[i].CP;
        MILL.speed              = TT[i].speed;
        MILL.CPbelongsToInitial = TT[i].CPbelongsToInitial;

        DETERMINE_CrossTimes(&MILL);

        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";

        if
        (
            fabs(MILL.CrossTimeStart - TT[i].expected_CrossStart) <=1e-5
            &&
            fabs(MILL.CrossTimeEnd   - TT[i].expected_CrossEnd) <=1e-5
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }


        cout << "  ||  ";
        cout << "exp START: "   << TT[i].expected_CrossStart << "  ";
        cout << "obs START: "   << MILL.CrossTimeStart       << " || ";
        cout << "exp END: "     << TT[i].expected_CrossEnd   << "  ";
        cout << "obs END: "     << MILL.CrossTimeEnd         << " || ";
        cout << endl;
    }
    return -1;
}



int TEST__DETERMINE_Overleap(void)
{
    cout << "\n";
    cout << " >>> " << "\n";
    cout << " >>> TEST FOR: DetermineOverleap() " << "\n";
    cout << " >>> " << "\n";
    cout << "\n";

    struct TESTTable
    {
        double T1_CrossStart;
        double T1_CrossEnd;
        double T2_CrossStart;
        double T2_CrossEnd;
        int    ExpectedOverleap;
    };

    int AmountOfTest = 14;
    TESTTable TT[AmountOfTest] =   {
                                         1,  2,  1,  2, overleapExist_YES ,
                                         0,  6,  1,  2, overleapExist_YES ,
                                         0,  1,  1,  2, overleapExist_YES ,
                                         2,  3,  1,  2, overleapExist_YES ,

                                         3,  6,  1,  2, overleapExist_NO  ,
                                       0.1,0.5,  1,  2,  overleapExist_NO ,

                                         1,  2,  1,  2, overleapExist_YES ,
                                         1,  2,  0,  6, overleapExist_YES ,
                                         1,  2,  0,  1, overleapExist_YES ,
                                         1,  2,  2,  3, overleapExist_YES ,

                                         1,  2,  3,  6, overleapExist_NO  ,
                                         1,  2,0.1,0.5,  overleapExist_NO ,


                                         2,  3,  0,  2, overleapExist_YES ,
                                        70, 75, 80, 85, overleapExist_NO,

                                    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL1;
        MILLPEDE MILL2;
        MILL1.CrossTimeStart = TT[i].T1_CrossStart;
        MILL1.CrossTimeEnd   = TT[i].T1_CrossEnd;
        MILL2.CrossTimeStart = TT[i].T2_CrossStart;
        MILL2.CrossTimeEnd   = TT[i].T2_CrossEnd;


        DETERMINE_Overleap(&MILL1, &MILL2);


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";

        if(MILL1.TimesOverleap == TT[i].ExpectedOverleap)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout << "  ||  ";
        cout << "Start: " <<MILL1.CrossTimeStart <<", ";
        cout << "End: "<<MILL1.CrossTimeEnd <<", ";
        cout << "Start: " <<MILL2.CrossTimeStart <<", ";
        cout << "End: "<<MILL2.CrossTimeEnd <<",   ||  result: ";
        if(MILL1.TimesOverleap) cout << "Overleap_YES";
        else cout << "Overleap_NO";

        cout << endl;
    }

    return -1;
}



int TEST__DETERMINE_ShiftMatrix(void)
{
    int AmountOfTest = 4;
    struct TESTTable
    {
    Point   P1;
    Point   P2;
    LINFAC  lin;
    Shift_M expected_sT;
    };

    cout <<                                             "\n";
    cout << " >>> " <<                                  "\n";
    cout << " >>> TEST for DETERMINE_ShiftMatrix()" <<  "\n";
    cout << " >>> " <<                                  "\n";
    cout <<                                             "\n";

    TESTTable TT[AmountOfTest] = 
    {
    //-----------------| |-----------------| |----------------|
    //        |        | |     LINFAC      | | ExpectedMatrix |
    //   P1   |   P2   | |  A  |  B  |  C  | |   Tx   |  Ty   |
    //        |        | |     |     |     | |        |       |
       -2,2   , -2, 4  ,      1,    0,    2,         2,0,
       10,0   , 10,10  ,      1,    0,  -10,       -10,0,
        0,4   ,  0, 2  ,      1,    0,    0,         0,0,
        3,4   ,  3,-2  ,      1,    0,   -3,        -3,0,
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        Shift_M sT;
        MILLPEDE MILL;
        MILL.lin = TT[i].lin;

        DETERMINE_ShiftMatrix(&MILL, &sT);

        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";

        if
        (
             (fabs(sT.Tx - TT[i].expected_sT.Tx ) <= 1e-10)
             &&
             (fabs(sT.Ty - TT[i].expected_sT.Ty ) <= 1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }



        #if MODE_DEB == 1 
        stringstream buf_1, buf_2, buf_3, buf_4;
        string temp = "                              ";

        buf_1   << "  ||  "
                << "P1("<<TT[i].P1.x<<","<<TT[i].P1.y<<")"
                << "P2("<<TT[i].P2.x<<","<<TT[i].P2.y<<")"
                << "    ";
        string BUF_1 = buf_1.str();

        buf_2   << "  (A,B,C): ("
                << MILL.lin.A << ","
                << MILL.lin.B << ","
                << MILL.lin.C << ")";
        string BUF_2 = buf_2.str();

        buf_3   << "(Tx,Ty,): EXP: ("
                << TT[i].expected_sT.Tx << ","
                << TT[i].expected_sT.Ty << ")";
        string BUF_3 = buf_3.str();
             
        buf_4   << "  OBS: ("
                << sT.Tx << ","
                << sT.Ty << ")";
        string BUF_4 = buf_4.str();

        temp.replace( 0,1,BUF_1);
        temp.replace(23,1,BUF_2);
        temp.replace(45,1,BUF_3);
        temp.replace(75,1,BUF_4);

        cout << temp;
        cout << endl;
        #else
        #endif
    }   return -1;
}



int TEST__DETERMINE_RotateMatrix(void)
{
    int AmountOfTest = 3;
    struct TESTTable
    {
    Point       P1;
    Point       P2;
    LINFAC      lin;
    Rotate_M    exp_RT;
    };

    cout <<                                              "\n";
    cout << " >>> " <<                                   "\n";
    cout << " >>> TEST for DETERMINE_RotateMatrix()" <<  "\n";
    cout << " >>> " <<                                   "\n";
    cout <<                                              "\n";

    TESTTable TT[AmountOfTest] = 
    {
    //-----------------| |-----------| |-----------------------------------|
    //        |        | |  LINFAC   | |          Expected MATRIX          |
    //   P1   |   P2   | | A | B | C | | theta |     M11     |     M21     |
    //        |        | |   |   |   | |       |     M12     |     M22     |
        0,1   ,   0,2  ,   1 , 0 , 0 ,  -M_PI/2, cos(M_PI/2) , -sin(M_PI/2),
                                                 sin(M_PI/2) ,  cos(M_PI/2),

        0,-2  ,   0,4  ,   1 , 0 , 0 ,  -M_PI/2, cos(M_PI/2) , -sin(M_PI/2),
                                                 sin(M_PI/2) ,  cos(M_PI/2),

        1,1   ,   3,3  ,   1 ,-1 , 0 ,  -M_PI/4,cos(M_PI/4) ,-sin(M_PI/4) ,
                                                sin(M_PI/4) , cos(M_PI/4) ,

    };


    for(int i=0; i<AmountOfTest; i++)
    {
        Rotate_M    RT;
        Rotate_M    T;
        MILLPEDE    MILL;
        MILL.lin =  TT[i].lin;

        DETERMINE_RotateMatrix(&MILL, &RT, &T);
 
        cout << "TC" << i+1 << ": ";

        if
        (
             (fabs(RT.T[0][0] - TT[i].exp_RT.T[0][0] ) <= 1e-10)
             &&
             (fabs(RT.T[0][1] - TT[i].exp_RT.T[0][1] ) <= 1e-10)
             &&
             (fabs(RT.T[1][0] - TT[i].exp_RT.T[1][0] ) <= 1e-10)
             &&
             (fabs(RT.T[1][1] - TT[i].exp_RT.T[1][1] ) <= 1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }



        #if     MODE_DEB == 1
        cout << endl;
        cout << endl;
        stringstream buf_1, buf_2, buf_3, buf_4, buf_5, buf_6, buf_7, buf_8, buf_9, buf_10;
        string line  = "                                                                                         ";
        string line1 = line;
        string line2 = line;
        cout    << endl;
        buf_1   << "|| "
                << "P1("<<TT[i].P1.x<<","<<TT[i].P1.y<<")";
        buf_2   << "P2("<<TT[i].P2.x<<","<<TT[i].P2.y<<")";
        buf_3   << "(A,B,C):("
                << MILL.lin.A << ","
                << MILL.lin.B << ","
                << MILL.lin.C << ")";
        buf_10  << "theta   EXP: "
                << TT[i].exp_RT.theta
                << "   OBS: "
                << RT.theta;


        buf_4   << "|| cos -sin | EXP: |"
                << TT[i].exp_RT.T[0][0] << ","
                << TT[i].exp_RT.T[0][1];
        buf_5   << "|| sin  cos |      |"
                << TT[i].exp_RT.T[1][0] << ","
                << TT[i].exp_RT.T[1][1];

        buf_6   << "| OBS: |"
                << RT.T[0][0] << ","
                << RT.T[0][1];
        buf_7   << "|      |"
                << RT.T[1][0] << ","
                << RT.T[1][1];

        buf_8   << "|";
        buf_9   << "|";

        string BUF_1 = buf_1.str();
        string BUF_2 = buf_2.str();
        string BUF_3 = buf_3.str();      
        string BUF_4 = buf_4.str();
        string BUF_5 = buf_5.str();
        string BUF_6 = buf_6.str();
        string BUF_7 = buf_7.str();
        string BUF_8 = buf_8.str();
        string BUF_9 = buf_9.str();
        string BUF_10 = buf_10.str();


        line1.replace(  0,1,BUF_1);
        line1.replace( 12,1,BUF_2);
        line1.replace( 20,1,BUF_3);
        line2.replace( 0,1,BUF_10);

        line1.replace( 38,1,BUF_4);
        line2.replace( 38,1,BUF_5);

        line1.replace( 85,1,BUF_6);
        line2.replace( 85,1,BUF_7);

        line1.replace(119,1,BUF_8);
        line2.replace(119,1,BUF_9);

        cout << line1 << endl;
        cout << line2 << endl;

        #else
        cout <<  " || theta (deg)  EXP: "
             << TT[i].exp_RT.theta * (180/3.141592)
             << "        OBS: "
             << RT.theta * (180/3.141592) << endl;
        #endif
    }   return -1;
}

int TEST__DETERMINE_LineCollisions(void)
{

    int AmountOfTest = 17;
    struct TESTTable
    {
        Point       ST1P1; 
        Point       ST1P2;
        double      ST1speed;
        Point       ST2P1;
        Point       ST2P2;
        double      ST2speed;
        LINCOL      exp_C0;
        LINCOL      exp_C1;
        LINCOL      exp_C2;
        LINCOL      exp_C3;
    };

    LINCOL   result[4];


    cout <<                                                "\n";
    cout << " >>> " <<                                     "\n";
    cout << " >>> TEST for DETERMINE_LineCollisions()" <<  "\n";
    cout << " >>> " <<                                     "\n";
    cout <<                                                "\n";


    TESTTable TT[AmountOfTest] =
    {
//==================================================================================================================
// PIONOWE NAKLADAJACE SIE - KOLIZJE BLISKO GRANICY OBSZARU GRY
//==================================================================================================================
//               |                |       |  exp        |   //
//       P1      |       P2       | speed |  collision  |   //  
//               |                |       |             |   //------------------------------------------------------------//
       -2, 0     ,     -1, 0      ,   1   ,                 // -TC1-                                                      //
        1, 0     ,      2, 0      ,   1   ,                 //           S1:1                             S2:1            //
                                        collisionNO,1,      //      -2         -1         0          1          2         //
                                        collisionNO,1,      //       P1         P2                   P1        P2         //
                                        collisionNO,1,      // ------|<========|----------|----------|<=========|----->   //           
                                        collisionNO,1,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//      
       -2, 0     ,     -1, 0      ,   1   ,                 // -TC2-                                                      //
        1, 0     ,      2, 0      ,   2   ,                 //           S:1                              S:2             //
                                        collisionYES,-5,    //      -2        -1          0          1         2          //
                                        collisionYES,-6,    //       P1        P2                    P1        P2         //
                                        collisionYES,-3,    // ------|<========|----------|----------|<=========|----->   //
                                        collisionYES,-4,    //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -2, 0     ,     -1, 0      ,   2   ,                 // -TC3-                                                      //
        1, 0     ,      2, 0      ,   1   ,                 //           S:2                              S:1             //
                                        collisionNO,0,      //      -2        -1          0          1         2          //
                                        collisionNO,0,      //       P1        P2                    P1        P2         //
                                        collisionNO,0,      // ------|<========|----------|----------|<=========|----->   //
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
        -4, 0     ,    -3, 0      ,   1   ,                 // -TC4-                                                      //
        -2, 0     ,    -1, 0      ,   1   ,                 //           S:1                              S:1             //
                                        collisionNO,0,      //      -4        -3                    -2        -1          //
                                        collisionNO,0,      //       P1        P2                    P1        P2     0   //
                                        collisionNO,0,      // ------|<========|---------------------|<=========|-----|-> //
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
        -4, 0     ,    -3, 0      ,   1   ,                 // -TC5-                                                      //
        -2, 0     ,    -1, 0      ,   2   ,                 //           S:1                              S:2             //
                                        collisionYES,-6,    //      -4        -3                    -2        -1          //
                                        collisionYES,-7,    //       P1        P2                    P1        P2     0   //
                                        collisionYES,-4,    // ------|<========|---------------------|<=========|-----|-> //
                                        collisionYES,-5,    //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
        -4, 0     ,    -3, 0      ,   2   ,                 // -TC6-                                                      //
        -2, 0     ,    -1, 0      ,   1   ,                 //           S:2                              S:1             //
                                        collisionNO,0,      //      -4        -3                    -2        -1          //
                                        collisionNO,0,      //       P1        P2                    P1        P2     0   //
                                        collisionNO,0,      // ------|<========|---------------------|<=========|-----|-> //
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
         1, 0     ,     2, 0      ,   1   ,                 // -TC7-                                                      //
         3, 0     ,     4, 0      ,   1   ,                 //           S:1                              S:1             //
                                        collisionNO,0,      //       1         2                     3         4          //
                                        collisionNO,0,      //  0    P1        P2                    P1        P2     0   //
                                        collisionNO,0,      // -|----|<========|---------------------|<=========|-----|-> //
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
         1, 0     ,     2, 0      ,   1   ,                 // -TC8-                                                      //
         3, 0     ,     4, 0      ,   2   ,                 //           S:1                              S:2             //
                                        collisionYES,-1,    //       1         2                     3         4          //
                                        collisionYES,-2,    //  0    P1        P2                    P1        P2         //
                                        collisionYES, 1,    // -|----|<========|---------------------|<=========|-------> //
                                        collisionYES, 0,    //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
         1, 0     ,     2, 0      ,   1   ,                 // -TC9-                                                      //
         3, 0     ,     4, 0      ,   1   ,                 //           S:1                              S:1             //
                                        collisionNO,0,      //       1         2                     3         4          //
                                        collisionNO,0,      //  0    P1        P2                    P1        P2         //
                                        collisionNO,0,      // -|----|<========|---------------------|<=========|-------> //
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
       -1, 0     ,     -2, 0      ,   1   ,                 // -TC10-                                                     //
        2, 0     ,      1, 0      ,   1   ,                 //           S1:1                             S2:1            //
                                        collisionNO,1,      //      -2         -1         0          1          2         //
                                        collisionNO,1,      //       P2        P1                    P2         P1        //
                                        collisionNO,1,      // ------|========>|----------|----------|=========>|----->   //           
                                        collisionNO,1,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -1, 0     ,     -2, 0      ,   2   ,                 // -TC11-                                                     //
        2, 0     ,      1, 0      ,   1   ,                 //           S1:2                             S2:1            //
                                        collisionYES,5,     //      -2         -1         0          1          2         //
                                        collisionYES,3,     //       P2        P1                    P2         P1        //
                                        collisionYES,6,     // ------|========>|----------|----------|=========>|----->   //           
                                        collisionYES,4,     //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -1, 0     ,     -2, 0      ,   1   ,                 // -TC12-                                                     //
        2, 0     ,      1, 0      ,   2   ,                 //           S1:1                             S2:2            //
                                        collisionNO,1,      //      -2         -1         0          1          2         //
                                        collisionNO,1,      //       P2        P1                    P2         P1        //
                                        collisionNO,1,      // ------|========>|----------|----------|=========>|----->   //           
                                        collisionNO,1,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -3, 0     ,     -4, 0      ,   1   ,                 // -TC13-                                                     //
       -1, 0     ,     -2, 0      ,   1   ,                 //           S1:1                            ST2:1            //
                                        collisionNO,1,      //      -4         -3                    -2         -1        //
                                        collisionNO,1,      //       P2        P1                    P2         P1   0    //
                                        collisionNO,1,      // ------|========>|---------------------|=========>|----|->  //          
                                        collisionNO,1,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -3, 0     ,     -4, 0      ,   2   ,                 // -TC14-                                                     //
       -1, 0     ,     -2, 0      ,   1   ,                 //          ST1:2                            ST2:1            //
                                        collisionYES,1,     //      -4         -3                    -2         -1        //
                                        collisionYES,-1,    //       P2        P1                    P2         P1   0    //
                                        collisionYES,2,     // ------|========>|---------------------|=========>|----|->  //          
                                        collisionYES,0,     //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -3, 0     ,     -4, 0      ,   1   ,                 // -TC15-                                                     //
       -1, 0     ,     -2, 0      ,   2   ,                 //           S1:1                             S2:2            //
                                        collisionNO,1,     //      -4         -3                    -2         -1        //
                                        collisionNO,1,     //       P2        P1                    P2         P1   0    //
                                        collisionNO,1,     // ------|========>|---------------------|=========>|----|->  //          
                                        collisionNO,1,     //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
//----------------------------------------------------------//------------------------------------------------------------//
       -1, 0     ,     -2, 0      ,   1   ,                 // -TC16-                                                     //
        1, 0     ,      2, 0      ,   1   ,                 //           S1:1                             S2:1            //
                                        collisionYES,0,     //      -2         -1         0          1          2         //
                                        collisionYES,0.5,   //       P2        P1                    P1         P2        //
                                        collisionYES,-0.5,  // ------|========>|----------|----------|<=========|----->   //           
                                        collisionYES,0,     //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
       -2, 0     ,     -1, 0      ,   1   ,                 // -TC17-                                                     //
        2, 0     ,      1, 0      ,   1   ,                 //           S1:1                             S2:1            //
                                        collisionNO,0,      //      -2         -1         0          1          2         //
                                        collisionNO,0,      //       P1        P2                    P2         P1        //
                                        collisionNO,0,      // ------|<========|----------|----------|=========>|----->   //           
                                        collisionNO,0,      //                                                            //
//----------------------------------------------------------//------------------------------------------------------------//
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE    ST1;
        MILLPEDE    ST2;

        ST1.P1    =  TT[i].ST1P1;
        ST1.P2    =  TT[i].ST1P2;
        ST1.speed =  TT[i].ST1speed;
        ST2.P1    =  TT[i].ST2P1;
        ST2.P2    =  TT[i].ST2P2;
        ST2.speed =  TT[i].ST2speed;



        DETERMINE_LineCollisions(&ST1, &ST2, result);



        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
            (TT[i].exp_C0.Collision == collisionNO)
            &&
            (TT[i].exp_C1.Collision == collisionNO)
            &&
            (TT[i].exp_C2.Collision == collisionNO)
            &&
            (TT[i].exp_C3.Collision == collisionNO)

            &&

            (result[0].Collision == collisionNO)
            &&
            (result[1].Collision == collisionNO)
            &&
            (result[2].Collision == collisionNO)
            &&
            (result[3].Collision == collisionNO)
            
        )
        {
            TEST_PASS
        }
        else
        {
             if
            (
                (result[0].Collision == TT[i].exp_C0.Collision)
                &&
                (result[1].Collision == TT[i].exp_C1.Collision)
                &&
                (result[2].Collision == TT[i].exp_C2.Collision)
                &&
                (result[3].Collision == TT[i].exp_C3.Collision)

                &&

                (result[0].x == TT[i].exp_C0.x)
                &&
                (result[1].x == TT[i].exp_C1.x)
                &&
                (result[2].x == TT[i].exp_C2.x)
                &&
                (result[3].x == TT[i].exp_C3.x)
            )
            {
                TEST_PASS;
            }
            else
            {
                TEST_FAIL;
            }
        }


        #if     MODE_DEB == 1
        cout << endl;
        // cout << result[0].Collision << " | CPx: (" << result[0].x << ")" << endl;
        // cout << result[1].Collision << " | CPx: (" << result[1].x << ")" << endl;
        // cout << result[2].Collision << " | CPx: (" << result[2].x << ")" << endl;
        // cout << result[3].Collision << " | CPx: (" << result[3].x << ")" << endl;
        cout << endl;
        #else
        cout << endl;
        #endif

    }
    return -1;
}



int TEST__TRANSFORM_MillpedeToHorizontalMillpede(void)
{

    int AmountOfTest = 36;
    struct TESTTable
    {
        Point       P1;
        Point       P2;
        Point       exp_P1;
        Point       exp_P2;
    };

    cout <<                                                                 "\n";
    cout << " >>>                                                   " <<    "\n";
    cout << " >>> TEST for: TRANSFORM_VerticalToHorizontalMillpede()" <<    "\n";
    cout << " >>>                                                   " <<    "\n";   
    cout << " >>>   Depends on: DETERMINE_LineEquation              " <<    "\n";
    cout << " >>>                                                   " <<    "\n";
    cout <<                                                                 "\n";

    TESTTable TT[AmountOfTest] = 
    {
    //|-----------------||---------------------------| 
    //|        |        ||   EXPECTED     EXPECTED   | 
    //|   P1   |   P2   ||      P1    |      P2      | 
    //|        |        ||            |              | 
         0,1   ,   0,2  ,         1,0 ,          2,0 ,  //1
         0,-2  ,   0,4  ,        -2,0 ,          4,0 ,  //2
         1,5   ,   1,2  ,         5,0 ,          2,0 ,  //3
        -1,5   ,  -1,2  ,         5,0 ,          2,0 ,  //4
        -9,5   ,  -9,2  ,         5,0 ,          2,0 ,  //5

         1,1   ,   3,3  ,   sqrt(2),0 ,   sqrt(18),0 ,//6
         0,1   ,   2,3  ,   sqrt(2),0 ,   sqrt(18),0 ,//7
        -1,1   ,   1,3  ,   sqrt(2),0 ,   sqrt(18),0 ,//8
        -2,1   ,   0,3  ,   sqrt(2),0 ,   sqrt(18),0 ,//9
        -3,1   ,  -1,3  ,   sqrt(2),0 ,   sqrt(18),0 ,//1

         6,2   ,   2,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//11
         4,2   ,   0,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//12
         2,2   ,  -2,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//13
         0,2   ,  -4,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//14
        -2,2   ,  -6,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//15
        
         2,2   ,   6,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//16
         0,2   ,   4,-2 ,   sqrt(8),0 ,   -sqrt(8),0 ,//17
        -2,2   ,   0,-2 ,   sqrt(5),0 ,   -sqrt(5),0 ,//18
        -4,2   ,  -2,-2 ,   sqrt(5),0 ,   -sqrt(5),0 ,//19
        -6,2   ,  -4,-2 ,   sqrt(5),0 ,   -sqrt(5),0 ,//20
        
        -2, 3  ,  -4, 1 ,  sqrt(18),0 ,   sqrt(2 ),0 ,//21
         4, 3  ,   2, 1 ,  sqrt(18),0 ,   sqrt(2 ),0 ,//22
        -2,-1  ,  -4,-3 , -sqrt(2 ),0 ,  -sqrt(18),0 ,//23
         4,-1  ,   2,-3 , -sqrt(2 ),0 ,  -sqrt(18),0 ,//24

        -4, 3  ,  -2, 1 ,  sqrt(18),0 ,   sqrt(2 ),0 ,//25
         2, 3  ,   4, 1 ,  sqrt(18),0 ,   sqrt(2 ),0 ,//26
        -4,-1  ,  -2,-3 , -sqrt(2 ),0 ,  -sqrt(18),0 ,//27
         2,-1  ,   4,-3 , -sqrt(2 ),0 ,  -sqrt(18),0 ,//28


     99999,-99998   ,    99999,-99999    ,-99998,0,    -99999,0    ,      //29
     99999,-99997   ,    99999,-99996    ,-99997,0,    -99996,0    ,     //30
     99999, 99998   ,    99999, 99999    , 99998,0,     99999,0    ,    //31
     99999, 99997   ,    99999, 99996    , 99997,0,     99996,0    ,   //32
    -99999, 99998   ,   -99999, 99999    , 99998,0,     99999,0    ,  //33
    -99999, 99997   ,   -99999, 99996    , 99997,0,     99996,0    , //34
    -99999,-99998   ,   -99999,-99999    ,-99998,0,    -99999,0    ,//35
    -99999,-99997   ,   -99999,-99996    ,-99997,0,    -99996,0    //36


    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE    MILL;
        MILLPEDE    h_MILL;
        Shift_M     sT;
        Rotate_M    rT;
        Rotate_M    rT_Reet;


        MILL.P1 =  TT[i].P1;
        MILL.P2 =  TT[i].P2;


        DETERMINE_LineEquation(&MILL);
        DETERMINE_ShiftMatrix( &MILL, &sT);
        DETERMINE_RotateMatrix(&MILL, &rT, &rT_Reet);


        TRANSFORM_MillpedeToHorizontalMillpede
            ( 
                  &MILL, 
                   sT, 
                   rT, 
                  &h_MILL 
            );


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
             (fabs(h_MILL.P1.x - TT[i].exp_P1.x ) <= 1e-10)
             &&
             (fabs(h_MILL.P1.y - TT[i].exp_P1.y ) <= 1e-10)
             &&
             (fabs(h_MILL.P2.x - TT[i].exp_P2.x ) <= 1e-10)
             &&
             (fabs(h_MILL.P2.y - TT[i].exp_P2.y ) <= 1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }



        #if     MODE_DEB == 1
        cout << endl;
        cout << "from: ";
        cout << "P1("<<MILL.P1.x<<","<<MILL.P1.y<<")";
        cout << "P2("<<MILL.P2.x<<","<<MILL.P2.y<<")";
        cout << endl;
        cout << " exp: ";
        cout << "P1("<<TT[i].exp_P1.x<<","<<TT[i].exp_P1.y<<")";
        cout << "P2("<<TT[i].exp_P2.x<<","<<TT[i].exp_P2.y<<")";
        cout << endl;
        cout << " obs: ";
        cout << "P1("<<h_MILL.P1.x<<","<<h_MILL.P1.y<<")";
        cout << "P2("<<h_MILL.P2.x<<","<<h_MILL.P2.y<<")";
        cout << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Linfac (A,B,C): " << MILL.lin.A << "," << MILL.lin.B << "," << MILL.lin.C << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Shift_M: " << endl;
        cout << sT.Tx << endl;
        cout << sT.Ty << endl;
        cout << "-----------------------------------------" << endl;
        cout << "theta rad : " << rT.theta << endl;
        cout << "theta deg : " << rT.theta * (180/3.141592) << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Rotate_M: " << endl;
        cout << rT.T[0][0] << "              " << rT.T[1][0] << endl; // [kolumny][wiersze]
        cout << rT.T[0][1] << "              " << rT.T[1][1] << endl; // [kolumny][wiersze]
        cout << "-----------------------------------------" << endl;
        cout << " obs: ";
        cout << "P1("<<h_MILL.P1.x<<","<<h_MILL.P1.y<<")";
        cout << "P2("<<h_MILL.P2.x<<","<<h_MILL.P2.y<<")" << endl;
        cout << "-----------------------------------------" << endl;
        cout << endl;
        cout << endl;
        cout << endl;
        #else
        cout << "  || theta: " << rT.theta * (180/3.141592);
        gotox((short)30);
        cout << "  ||  from: "
             << "P1("<<MILL.P1.x<<","<<MILL.P1.y<<")"
             << "P2("<<MILL.P2.x<<","<<MILL.P2.y<<")";
        gotox((short)80);
        cout << " exp: "
             << "P1("<<TT[i].exp_P1.x<<","<<TT[i].exp_P1.y<<")"
             << "P2("<<TT[i].exp_P2.x<<","<<TT[i].exp_P2.y<<")";
        gotox((short)120);
        cout << " obs: "
             << "P1("<<h_MILL.P1.x<<","<<h_MILL.P1.y<<")"
             << "P2("<<h_MILL.P2.x<<","<<h_MILL.P2.y<<")"
             << endl;
        #endif
    }   return -1;
}



int TEST__CheckMillipedeCollision(void)
{
    
    struct TESTTable
    {
    Point   S1P1;
    Point   S1P2;
    double  speed1;
    Point   S2P1;
    Point   S2P2;
    double  speed2;
    int     expected_CollisionState;
    string  TestDescriptions;
    };

    int AmountOfTest = 104;
    TESTTable TT[AmountOfTest] =
    {
    //=========================================================
    //        |        |       |  exp        |
    //   P1   |   P2   | speed |  collision  |  Descriptions
    //        |        |       |             |
        2,  0,  0,  0,  1,    // TC 1
        4, -2,  4, -4,  1, collisionYES, "TC101 kolizja - przeciecie po czasie",

       -1,  2, -3,  2,  2,   //  TC 2
        2,  1,  2, -4,  1, collisionYES,"brak",

       -1,  2, -3,  2,  2,   //  TC 3
       -1,  1, -3,  1,  1, collisionNO,"brak",

       -2, -2, -4,  0,  1,   //  TC 4
        1, -1, -4,  -7, 1, collisionYES,"brak",

       -4, 0, -2,  -2,  1,   //  TC 5
        1, -1, -4,  -7, 1, collisionNO,"brak",

       -2, -2, -4,   0, 1,   //  TC 6
        1, -1, -4,  -7, 1, collisionYES,"Zderzenie = punkt zderzenia w miejscu startu drugiej stonogi",

        0,  1, -1,   1, 1,   //  TC 7
        1,  0,  1,  -1, 1, collisionYES,"Zderzenie po 1 sek, proste prostopadle",

        1,  2,  0,   2, 1,   //  TC 8
        2,  1,  2,   0, 1, collisionYES,"Zderzenie po 1 sek, proste prostopadle",

       -2, -2, -4,   0,99,   //  TC 9
        1, -1, -4,  -7, 1, collisionYES,"Zderzenie = punkt zderzenia w miejscu startu drugiej stonogi, pocisk",

    -4.52, 6.28,  4.3,  3.64,  0.01, //TC10
    -4.67, 3.43,-6.49,  1.73,     1, collisionYES,"kolizja - przeciecie po czasie",

    -4.52, 6.28,  4.3,  3.64,  0.5,  //TC11
    -4.67, 3.43,-6.49,  1.73,   200, collisionYES,"kolizja - przeciecie po czasie" ,

    -4.52, 6.28,  4.3,  3.64,    50, //TC12
    -4.67, 3.43,-6.49,  1.73,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

    -4.52, 6.28,  4.3,  3.64,  0.01, //TC13
    -6.49, 1.73,-4.67,  3.43,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

        0,    2,    0,     0,    10, //TC14
        2,    2,    2,     0,     1, collisionNO,"brak przeciecia Trajektorie rownolegle pionowe",

        3,    3,   -3,     3,   100, //TC15
        3,   -3,   -3,    -3,    10, collisionNO,"brak przeciecia Trajektorie rownloegle poziome",

//|==============================================================================================================|
//| TRAJEKTORIE POZIOME NAKLADAJACE SIE                                                                     |
//|==============================================================================================================|
//|        |        |       |  exp        |                                                                      |
//|   P1   |   P2   | speed |  collision  |  Descriptions                                                        |
//|        |        |       |             |                                                                      |
     -1,1  , -2,1   ,   1   , //TC16
      1,1  ,  2,1   ,   1   , collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, zderzenie czolowe po czasie 1",

     -2,1  ,  -1,1  ,   1   , //TC17
      1,1  ,   2,1  ,   1   , collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, poruszanie w lewe ze stala predkoscia",

     -2,1  ,  -1,1  ,   1   , //TC18
      1,1  ,   2,1  ,   2   , collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

     -2,1  ,  -1,1  ,   1   , //TC19
      1,1  ,   2,1  ,  0.5  , collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

//==============================================================================================================
// TRAJEKTORIE PIONOWE NAKLADAJACE SIE
//==============================================================================================================
//        |        |       |  exp        |
//   P1   |   P2   | speed |  collision  |  Descriptions
//        |        |       |             |
    0, 1  ,  0, 2  ,   1   , //TC20 // kolizja w CP (0,0) po czasie 1
    0,-1  ,  0,-2  ,   1   , collisionYES,"     KOL - Trajektorie PIONOWE sie nakladaja, zderzenie czolowe po czasie 1",

    0, 1  ,  0, 2  ,   1   , //TC21 //
    0,-2  ,  0,-1  ,   1   , collisionNO, "BRAK KOL - Trajektorie PIONOWE sie nakladaja, poruszanie w dol ze stala predkoscia",

    0, 1  ,  0, 2  ,   2   , //TC22 // kolizja w CP (0,3) po czasie 2
    0,-2  ,  0,-1  ,   1   , collisionYES, "     KOL - Trajektorie PIONOWE sie nakladaja, kol w CP(0,3) po 2s || ST1 dogoniÅ‚a ST2",

    0, 1  ,  0, 2  ,  0.3  , //TC23 //
    0,-2  ,  0,-1  ,   1   , collisionNO, "BRAK  KOL - Trajektorie PIONOWE sie nakladaja, ST1 nigdy nie dogini ST@",
//==============================================================================================================
// KOLIZJA SKOSNA, KOLIZJA GRANICZNA (KONIC Z POCZATKIEM)
//==============================================================================================================
//        |        |       |  exp        |
//   P1   |   P2   | speed |  collision  |  Descriptions
//        |        |       |             |
     0,0  ,  -1,1  ,   1   , // TC24
     0,1  ,   1,2  ,   1   , collisionYES,"KOLIZJA SKOSšNA, KOLIZJA GRANICZNA (KONIC Z POCZATKIEM)",

//==============================================================================================================
// POZIOME NAKLADAJACE SIE - KOLIZJE BLISKO GRANICY OBSZARU GRY
//==============================================================================================================
//               |                |       |  exp        |
//       P1      |       P2       | speed |  collision  |  Descriptions
//               |                |       |             |
   -99998, 99999 ,  -99999, 99999 ,   1   , // TC25
   -99997, 99999 ,  -99996, 99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",
    99998, 99999 ,   99999, 99999 ,   1   , // TC26
    99997, 99999 ,   99996, 99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",
    99998,-99999 ,   99999,-99999 ,   1   , // TC27
    99997,-99999 ,   99996,-99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",
   -99998,-99999 ,  -99999,-99999 ,   1   , // TC28
   -99997,-99999 ,  -99996,-99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",

   -99998, 0     ,  -99999, 0     ,   1   , // TC29
   -99997, 0     ,  -99996, 0     ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",
    99998, 0     ,   99999, 0     ,   1   , // TC30
    99997, 0     ,   99996, 0     ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",

   -99998, 99999 ,  -99999, 99999 ,   1   , // TC31
    99998, 99999 ,   99999, 99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",
   -99998,-99999 ,  -99999,-99999 ,   1   , // TC32
    99998,-99999 ,   99999,-99999 ,   1   , collisionYES,"TRAJEKTORIE POZIOME, KOLIKZJA BLISKO GRANICY",


//==============================================================================================================
// PIONOWE NAKLADAJACE SIE - KOLIZJE BLISKO GRANICY OBSZARU GRY
//==============================================================================================================
//               |                |       |  exp        |
//       P1      |       P2       | speed |  collision  |  Descriptions
//               |                |       |             |
    99999,-99998 ,  99999,-99999  ,   1   , // TC33
    99999,-99997 ,  99999,-99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",
    99999, 99998 ,  99999, 99999  ,   1   , // TC34
    99999, 99997 ,  99999, 99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",
   -99999, 99998 , -99999, 99999  ,   1   , // TC35
   -99999, 99997 , -99999, 99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",
   -99999,-99998 , -99999,-99999  ,   1   , // TC36
   -99999,-99997 , -99999,-99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",

        0,-99998 ,      0,-99999  ,   1   , // TC37
        0,-99997 ,      0,-99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",
        0, 99998 ,      0, 99999  ,   1   , // TC38
        0, 99997 ,      0, 99996  ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",

    99999, -99998 ,  99999,-99999 ,   1   , // TC39 kolizja w CP (99999,0) 
    99999,  99998 ,  99999, 99999 ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",
   -99999, -99998 , -99999,-99999 ,   1   , // TC40
   -99999,  99998 , -99999, 99999 ,   1   , collisionYES,"TRAJEKTORIE PIONOWE, KOLIKZJA BLISKO GRANICY",

//==============================================================================================================
// cases form the internet
//==============================================================================================================
//               |                |       |  exp        |
//       P1      |       P2       | speed |  collision  |  Descriptions
//               |                |       |             |

        0, 5      ,      0, 8      ,   3   ,// TC41 kolizja w CP (0,0) 
        1, 1      ,      3, 3      ,   1   , collisionYES,"Similar coeficientds (A,B,C) ST1:(1,0,0)  ST2:(1,-1,0)",

        0, 5      ,      0, 8      ,   3   ,// TC42 kolizja w CP (0,0) 
       -1, 1      ,     -3, 3      ,   1   , collisionYES,"Similar coeficientds (A,B,C) ST1:(1,0,0)  ST2:(-1,-1,0)",
        
        0, 5      ,      0, 8      ,   3   ,// TC43 kolizja w CP (0,0) 
       -1,-1      ,     -3,-3      ,   1   , collisionYES,"Similar coeficientds (A,B,C) ST1:(1,0,0)  ST2:(1,-1,0)",
        
        0, 5      ,      0, 8      ,   3   ,// TC44 kolizja w CP (0,0) 
        1,-1      ,      3,-3      ,   1   , collisionYES,"Similar coeficientds (A,B,C) ST1:(1,0,0)  ST2:(-1,-1,0)",
        
       -1,-1      ,     -3,-3      ,   1   ,// TC45
        1,-1      ,      3,-3      ,   3   , collisionYES,"Case form the internet",
        
        1, 1      ,      3, 3      ,   1   ,// TC46
       -1, 1      ,     -3, 3      ,   3   , collisionYES,"Case form the internet",
        
        1, 1      ,     10,10      ,   1   ,// TC47
       -1, 1      ,    -10,10      ,  10   , collisionYES,"Case form the internet",

        0,99999   ,      0,99998   ,   2   ,// TC48
        0,0       ,      0,-100    ,100000 , collisionYES, "Collision at playZone border",

    99999,99999   ,  99998,99998   ,sqrt(8)         ,// TC49
        0,0       ,   -100,-100    ,sqrt(2) * 1e5  , collisionYES, "Collision at playZone border",



        3, 7      ,      2,  7     ,   1   ,// TC50
     -100, 3      ,     20,  3     ,   5   , collisionNO,"Case form the internet",
        2, 2      ,      4,  2     ,   2   ,// TC51
      -90, 100000 ,     90,  100000,  100  , collisionNO,"Case form the internet",
        1, 100000 ,     -5,  100000,   3   ,// TC52
        1,-100000 ,     -5,-100000 ,   3   , collisionNO,"Case form the internet",
        3, 0      ,      5,  0     ,   3   ,// TC53
       10,-100000 ,     12,-100000 ,   4   , collisionNO,"Case form the internet",
        3, 7      ,      3,-7      ,   2   ,// TC54
        0,-5      ,      0, 5      ,   1   , collisionNO,"Case form the internet",
   100000, 3      , 100000, 4      ,   7   ,// TC55
  -100000, 2      ,-100000, 7      ,   1   , collisionNO,"Case form the internet",
        0, 7      ,      0,-7      ,   1   ,// TC56
       -1,-1      ,     -1, 1      ,   1   , collisionNO,"Case form the internet",
  -100000, 3      ,-100000, 4      ,   2   ,// TC57
   -99999, 2      , -99999, 3      ,   1   , collisionNO,"Case form the internet",
        0, 1      ,      7, 8      ,   3   ,// TC58
       -3,-3      ,      5, 5      ,   7   , collisionNO,"Case form the internet",
        1, 0      ,      2, 100000 ,   3   ,// TC59
        0, 100000 ,     -1, 0      ,   2   , collisionNO,"Case form the internet",
        1, 10     ,     -1, 4      ,   2   ,// TC60
        3,-91     ,    -30,-190    ,   5   , collisionNO,"Case form the internet",
       10, 66670  ,     -8, 66664  ,   1   ,// TC61
        0, 60000  ,     -5, 59998  ,   2   , collisionNO,"Case form the internet",
      -10, 60006  ,      0, 60000  ,   3   ,// TC62
        0,-200000 ,     -5,-200010 ,   4   , collisionNO,"Case form the internet",
        0, 0      ,     -1, 1      ,   5   ,// TC63
        5, 99990  ,      0, 100000 ,   6   , collisionNO,"Case form the internet",
        8,-99972  ,      0,-100000 ,   7   ,// TC64
        3,-100005 ,      0,-10000  ,   8   , collisionYES,"Case form the internet",
        0, 0      ,  -100 , 100    ,   9   ,// TC65
       10, 49995  ,    100, 49950  ,  10   , collisionNO,"Case form the internet",
        0, 100000 ,      1, 100002 ,  11   ,// TC66
        0, 100000 ,      1, 100001 ,  12   , collisionYES,"Case form the internet",
        0,-99999  ,      1,-99997  ,   1   ,// TC67
        4, 25004  ,     -4, 24998  ,   2   , collisionNO,"Case form the internet",

       -1, 1      ,     -5, 5      ,   2   ,// TC68
        1, 1      ,      2, 2      ,   3   , collisionYES,"Case form the internet", 
       -1, 1      ,     -5, 5      ,   2   ,// TC69
        1, 1      ,      2, 2      ,   15  , collisionNO,"Case form the internet",
       -1, 1      ,     -5, 5      ,   2   ,// TC70
       -1,-1      ,     -2,-2      ,   3   , collisionYES,"Case form the internet",
       -1, 1      ,     -5, 5      ,   2   ,// TC71
        2, 2      ,      1, 1      ,   3   , collisionNO,"Case form the internet",
       -1, 1      ,     -5, 5      ,   2   ,// TC72
       -2,-2      ,     -1,-1      ,   3   , collisionNO,"Case form the internet",

        0, 5      ,      0, 8      ,   3   , // TC73
        1, 1      ,      3, 3      ,   1   , collisionYES,"Case form the internet",
        0, 8      ,      0, 5      ,   3   , // TC74
        1, 1      ,      3, 3      ,   1   , collisionNO,"Case form the internet",
        0, 5      ,      0, 8      ,   3   , // TC75
        3, 3      ,      1, 1      ,   1   , collisionNO,"Case form the internet",
        0, 8      ,      0, 5      ,   3   , // TC76
        3, 3      ,      1, 1      ,   1   , collisionNO,"Case form the internet",
        0, 5      ,      0, 8      ,   3   , // TC77
        1, 1      ,      3, 3      ,   5   , collisionNO,"Case form the internet",
        1, 1      ,      3, 3      ,   1   , // TC78
        0, 5      ,      0, 8      ,   3   , collisionYES,"Case form the internet",
        0, 5      ,      0, 8      ,   3   , // TC79
        5, 0      ,      8, 0      ,   3   , collisionYES,"Case form the internet",
        0, 8      ,      0, 5      ,   3   , // TC80
        5, 0      ,      8, 0      ,   3   , collisionNO,"Case form the internet",
        8, 0      ,      5, 0      ,   3   , // TC81
        0, 8      ,      0, 5      ,   3   , collisionNO,"Case form the internet",
        5, 0      ,      8, 0      ,   3   , // TC82
        0, 5      ,      0, 8      ,   3   , collisionYES,"Case form the internet",
        0, 5      ,      0, 8      ,   3   , // TC83
        1,-1      ,      3,-3      ,   1   , collisionYES,"Case form the internet",
        0, 8      ,      0, 5      ,   3   , // TC84
        1,-1      ,      3,-3      ,   1   , collisionNO,"Case form the internet",
       -5, 0      ,     -1, 0      ,   1   , // TC85
        0, 5      ,      0, 8      ,   1   , collisionNO,"Case form the internet",
       -1, 0      ,     -5, 0      ,   1   , // TC86
        0, 5      ,      0, 8      ,   1   , collisionYES,"Case form the internet",
       -1, 0      ,     -5, 0      ,   2   , // TC87
        0, 5      ,      0, 8      ,   1   , collisionNO,"Case form the internet",
       -1,-1      ,     -3,-3      ,   1   , // TC88
        1,-1      ,      3,-3      ,   1   , collisionYES,"Case form the internet",
       -1,-1      ,     -3,-3      ,   1   , // TC89
        1,-1      ,      3,-3      ,   3   , collisionYES,"Case form the internet",


   -99998,99999   , -99999, 99998  ,  100  , // TC90
    99998,99999   ,  99999, 99998  ,  100  , collisionNO,"Collision out of border", 

//==============================================================================================================
// cases form the internet
//==============================================================================================================
//               |                |       |  exp        |
//       P1      |       P2       | speed |  collision  |  Descriptions
//               |                |       |             |
       -1,-2     ,     -2,-1      ,   5   , // TC91
        1, 2     ,      2, 1      ,   5   , collisionNO,"Paraller",
       -1,-2     ,     -2,-1      ,   5   , // TC92
        2, 1     ,      1, 2      ,   5   , collisionNO,"Paraller",
        0, 3     ,      0, 2      ,   5   , // TC93
       -1, 3     ,     -1, 1      ,   5   , collisionNO,"Paraller",

        0, 0     ,      0, 1      ,   5   , // TC94
        1, 0     ,      2, 0      ,   5   , collisionYES,"perpendicular",
       -5, 4     ,     -6, 5      ,   5   , // TC95
       -4, 5     ,     -3, 6      ,   5   , collisionYES,"perpendicular",

        0, 100   ,      0,-100    ,   1   , // TC96
       10, 0     ,     20, 0      , 100   , collisionYES,"perpendicular BULLET",
       10, 0     ,     20, 0      , 100   , // TC97
        0, 100   ,      0,-100    ,   1   , collisionYES,"perpendicular BULLET",
       10, 0     ,     20, 0      , 100   , // TC98
       -1, 100   ,     -1,-100    ,   1   , collisionYES,"perpendicular BULLET",
       4, -2     ,      6,      -2,  20001, // TC99
      -6, -2     ,     -4,      -2,  20000, collisionNO,"From Acceptance Level tests",


//
//  https://www.wolframalpha.com/input?i=LINE%28234%2C678%29%28-187%2C+7%29%2C+LINE%28400%2C521%29%28847%2C-14%29
//
//
//          ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]
//
//              Line(234,678)(-187,  7)
//              Line(400,521)( 847,-14)
//
//  Cross Point: (130733899/525172, 368568017/525172)
//  S1         : (130733899/525172, 368568017/525172)(234,678) => (18631*sqrt(313741/2))/262586 => ST1Speed
//  ST1Speed   : 28.101871181544118163337635663378800411770253641351348151517850477
//  S2         : (130733899/525172, 368568017/525172)(847,-14) => (702655*sqrt(243017/2))/262586 => ST2Speed
//  ST2Speed   : 932.76856843563533431751621294926189576847334687997823834628097353
//
        234,678   ,   -187, 7      ,  (18631*sqrt(313741/2))/262586, // TC100
        400,521   ,    847,-14     , (702655*sqrt(243017/2))/262586, collisionYES,"ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]",
        234,678   ,   -187, 7      ,  (18631*sqrt(313741/2))/262586, // TC101
        400,521   ,    847,-14     , (702660*sqrt(243017/2))/262586, collisionNO,"ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]",
       234,678   ,   -187, 7      ,  28.10187118154411816333763566337, // TC102
       400,521   ,    847,-14     , 932.76856843563533431751621294926, collisionYES,"ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]",
       234,678   ,   -187, 7      ,  28.10187118154411816333763566337, // TC103
       400,521   ,    847,-14     , 932.76857, collisionNO,"ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]",
       234,678   ,   -187, 7      ,  29, // TC104
       400,521   ,    847,-14     , 932, collisionYES,"ST1 P1 CRASH WITH ST2 P2 AFTER 1[s]",

    };



    cout << "\n\n";
    cout << " ==========================================================================================================\n";
    cout << " >>>                                                                                                    <<<\n";
    cout << " >>> FINAL TESTS                                                                                        <<<\n";
    cout << " >>>                                                                                                    <<<\n";
    cout << " >>> TEST FOR: CheckMillipedeCollision()                                                                <<<\n";
    cout << " >>>                                                                                                    <<<\n";
    cout << " ==========================================================================================================\n";
    cout << "\n\n";

    for(int i=0; i<AmountOfTest; i++)
    {

        MILLPEDE MILL1;
        MILLPEDE MILL2;

        MILL1.P1  = TT[i].S1P1;
        MILL1.P2  = TT[i].S1P2;
        MILL1.speed = TT[i].speed1;

        MILL2.P1  = TT[i].S2P1;
        MILL2.P2  = TT[i].S2P2;
        MILL2.speed = TT[i].speed2;

        #if MODE_DEB == 1
        cout << "\n\n";
        cout << ">>>=========================================================<<<\n";
        cout << ">>>  MAIN_TC_DEBOUG: " << i+1 <<                           "<<<\n";
        cout << ">>>=========================================================<<<\n";
        cout << endl;
        cout << "    ST1(P1)(P2) >> "
             << "(" << TT[i].S1P1.x << "," << TT[i].S1P1.y << ")"
             << "(" << TT[i].S1P2.x << "," << TT[i].S1P2.y << ")" << endl;
        cout << "    ST2(P1)(P2) >> "
             << "(" << TT[i].S2P1.x << "," << TT[i].S2P1.y << ")"
             << "(" << TT[i].S2P2.x << "," << TT[i].S2P2.y << ")" << endl;
        cout << "    ------------------------------" << endl;
        cout << endl;
        #endif // MODE_DEB


        int ret = CHECK_MillipedeCollision(&MILL1, &MILL2);


        if(i+1<10)  cout << "TC" <<i+1<<" : ";
        else        cout << "TC" <<i+1<<": ";


        if
        (
           ret
           == 
           TT[i].expected_CollisionState
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }



        cout << "  ||  expected: ";
        if(collisionYES == TT[i].expected_CollisionState) cout << "TAK";
        else cout << "NIE";
        cout << "   observed: ";
        if(collisionYES == ret) cout << "TAK";
        else cout << "NIE";
        cout << "  ||  speed1: " << TT[i].speed1;
        gotox(90);
        cout << "  ||  Des: " << TT[i].TestDescriptions;
        cout << endl;

    }   return -1;
}

#endif // MODE_TEST