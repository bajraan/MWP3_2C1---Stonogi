#include <iostream>
#include <algorithm>
#include <math.h>
#include <sstream>

#define MODE_DEB     0
#define MODE_TEST    1
#define PRINT_RAPORT 0



#if     MODE_TEST == 1
#include <windows.h>
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#define TEST_PASS   SetConsoleTextAttribute(hConsole,10);cout<<"PASS";SetConsoleTextAttribute(hConsole,7);
#define TEST_FAIL   SetConsoleTextAttribute(hConsole,12);cout<<"FAIL";SetConsoleTextAttribute(hConsole,7);

#define FATAL_ERROR__DetermineCrossTimes SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineCrossTimes()\n";SetConsoleTextAttribute(hConsole,7);
#define FATAL_ERROR__DetermineOverleap   SetConsoleTextAttribute(hConsole,12);cout<<"Fatal Error - DetermineOverleap()\n";SetConsoleTextAttribute(hConsole,7);
#endif

#if     PRINT_RAPORT == 1
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

enum intersection
{
    intersection_NO,
    intersection_ONE,
    intersection_INF
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
    string  CrossPointExist;
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


// 1) trajektoria musi zosta† przesuni©ta do pukut 0,0
// 2) trajektoria musi zosta† obrucona wzgl©dem punkut 0,0
// utworzy† macierze transformacji;
// na bazie wzoru prostej

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
void DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2);
void DETERMINE_ShiftMatrix (MILLPEDE*T, Shift_M*sT);
void DETERMINE_RotateMatrix(MILLPEDE*T,Rotate_M*rT, Rotate_M* rT_Reet);
void DETERMINE_LineCollisions(MILLPEDE *T1, MILLPEDE *T2,LINCOL*tab);
void DETERMINE_SpeedSignCorrection(MILLPEDE *T);
int  CHECK_IF_CollisionPointbelongstoPlayZone(MILLPEDE *T, PLZ *PlayZone);
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
    ( 
     ST1->lin.A == ST2->lin.A
     &&
     ST1->lin.B == ST2->lin.B
     &&
     ST1->lin.C == ST2->lin.C 
    )
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


        //      TODO
        //      TODO
        //      TODO SPRAWDZANIE FLAGI    h_ST1.horizontalTransformed
        //      TODO
        //      TODO


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
                    Point x = {Lincoltable[i].x,0};
                    TRANSFORM_PointToOriginal(ST1, sT, rT_Reet, x);
                }


                if
                (
                CHECK_IF_CollisionPointbelongstoPlayZone(ST1, &PlayZone)
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
    (
    ST1->lin.A == ST2->lin.A
    &&
    ST1->lin.B == ST2->lin.B
    &&
    ST1->lin.C != ST2->lin.C
    )
    {
        return collisionNO;
    }
    else
    {

        DETERMINE_CrossPoint(ST1,ST2);

        if(ST1->CrossPointExist == "YES")
        {


            CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST1);


            if
            (
            CrossPointBelongsToInitial_FALSE
            ==
            ST1->CPbelongsToInitial
            )
            {
                DETERMINE_CrossDirection(ST1);
                if(neg == ST1->CPdirection ) return collisionNO;
            }


            CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST2);


            if
            (
            CrossPointBelongsToInitial_FALSE
            ==
            ST2->CPbelongsToInitial
            )
            {
                DETERMINE_CrossDirection(ST2);
                if(neg == ST2->CPdirection ) return collisionNO;
            }

            DETERMINE_CrossTimes(ST1);
            DETERMINE_CrossTimes(ST2);

            DETERMINE_Overleap(ST1,ST2);

            if
            (
            overleapExist_YES 
            == 
            ST1->TimesOverleap
            )
                    return collisionYES;
            else
                    return collisionNO;
        }
    else return collisionNO;
    }
    return -1;
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



    // LINIA PIONOWA
    if(T->P1.x == T->P2.x)
    {
        T->lin.A =  1;
        T->lin.B =  0;
        T->lin.C = -T->P1.x;
    }
    // LINIA POZIOMA
    else if(T->P1.y == T->P2.y)
    {
        T->lin.A = 0;
        T->lin.B = 1;
        T->lin.C = -T->P1.y;

    }
    else
    {

        double Slope;
        double Intercept;

    //
    // SLOPE // Ax
    //

        if(0 == (max(T->P1.y, T->P2.y) - min(T->P1.y, T->P2.y))) Slope = 0;
        else Slope = (T->P2.y - T->P1.y) / (T->P2.x - T->P1.x);
        T->lin.A = Slope;

    //
    // INTERCEPT // F
    //

        if(0 == Slope) Intercept = T->P1.x;
        else Intercept = T->P1.y - Slope * T->P1.x;
        T->lin.C = Intercept;

    //
    // By
    //

        T->lin.B = -1;

    }


    #if MODE_DEB == 1
    cout << " >> (A,B,C): " <<
    "("  << T->lin.A <<
    ","  << T->lin.B <<
    ","  << T->lin.C <<
    ")"  << endl;
    //cout << "B_: " << T->P1.y <<"-"<< T->lin.A <<"*"<< T->P1.x<< endl;
    #endif // MODE_DEB
}

int DETERMIEN_IntersectionScenario(MILLPEDE *ST1, MILLPEDE *ST2)
{
    return 0;
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

    T1->CrossPointExist = "YES";
    T2->CrossPointExist = "YES";


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
    cout << " >> sdfasfdsaf CP: (" << T1->CrossPoint.x <<","<<T1->CrossPoint.y <<")" << endl << endl;
    #endif // MODE_DEB
}


/// @brief 
/// @param T 
/// @param PlayZone 
/// @return 
int CHECK_IF_CollisionPointbelongstoPlayZone(MILLPEDE *T, PLZ *PlayZone)
{

    #if MODE_DEB == 1
    cout << "                   " << endl;
    cout << "CHECK_IF_CollisionPointbelongstoPlayZone" << endl;
    #endif // MODE_DEB


    if  (
        PlayZone->Min <= T->CrossPoint.x
        &&
        PlayZone->Max >= T->CrossPoint.x
        &&
        PlayZone->Min <= T->CrossPoint.y
        &&
        PlayZone->Max >= T->CrossPoint.y
        )
    {
        return CrossPointBelongsToPlayZone__TRUE;
    }
    else
    {
        return CrossPointBelongsToPlayZone_FALSE;
    }
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

    if      (cpp1 == cpp2) cout << endl << "FATAL ERROR" << endl;
    else if (cpp1 <  cpp2) T->CPdirection = pos;
    else if (cpp1 >  cpp2) T->CPdirection = neg;
    else                   cout << endl << "FATAL ERROR" << endl;


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
    else if(S1>S2)
    {
        T->CrossTimeStart = S2/T->speed;
        T->CrossTimeEnd   = S1/T->speed;
    }
    else
    {       
        #if          MODE_TEST == 1
        FATAL_ERROR__DetermineCrossTimes;
        #endif
    }       





    #if MODE_DEB == 1
    cout << "-----------------------------"  << endl;
    cout << "P1: (" << T->P1.x << "," << T->P1.y << ")" << endl;
    cout << "P2: (" << T->P2.x << "," << T->P2.y << ")" << endl;
    cout << "S1: " << S1 << endl;
    cout << "S2: " << S2 << endl;
    if(S1>S2)
    {
    cout << "CrossTimeStart:          >> "  << T->CrossTimeStart << endl;
    cout << "Calc: " << S2<<"/"<<T->speed   << endl;
    cout << "CrossTimeEnd:            >> "  << T->CrossTimeEnd << endl;
    cout << "Calc: " << S1<<"/"<<T->speed   << endl;
    }
    if(S1<S2)
    {
    cout << "CrossTimeStart:          >> "  <<T->CrossTimeStart << endl;
    cout << "Calc: " << S1<<"/" <<T->speed          << endl;
    cout << "CrossTimeEnd:            >> "  <<T->CrossTimeEnd   << endl;
    cout << "Calc: " << S2<<"/" <<T->speed          << endl;
    }
    cout << "-----------------------------"  << endl;
    cout << endl;
    #endif // MODE_DEB
}



void DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2)
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

}



void DETERMINE_ShiftMatrix(MILLPEDE*T, Shift_M *sT)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DETERMINE_ShiftMatrix";
    #endif // MODE_DEB


    if
    (                   // Case:
      1 == T->lin.A     // Vertical Trajectory
      &&                //
      0 == T->lin.B     //
    )                   //
    {
        sT->Tx = T->lin.C;
        sT->Ty = 0;

    }
    else if
    (
      0 > T->lin.A
    )
    {
        sT->Tx = -(T->lin.C/T->lin.A);
        sT->Ty = 0;
        //cout << endl << "CS2" << endl;
    }
    else
    {
        sT->Tx = T->lin.C;
        sT->Ty = 0;
        //cout << endl << "CS3" << endl;
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

        rT->theta = M_PI/2;   // -90 deg

    }
    else if(0 <= T->lin.A )                     //            /
    //==========================================//       |   /                  
                    // Angular Trajectory       //       |  /      
                    //  0deg < theta < 90deg    //       | /       
    //==========================================// --------------- 
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
    //      this matrix allows you fransform point.
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




//
//          ST1X1-ST2X1/V2-V1 = t 
//
void DETERMINE_LineCollisions(MILLPEDE *T1, MILLPEDE *T2, LINCOL* tab)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DETERMINE_LineCollisions";
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
        
        t = (T1->P1.x - T2->P1.x)/(T1->speed - T2->speed);
        if(t<0)     tab[0].Collision = collisionNO;
        else        tab[0].x = T1->P1.x + T1->speed * t; 

        t = (T1->P1.x - T2->P2.x)/(T1->speed - T2->speed);
        if(t<0)     tab[1].Collision = collisionNO;
        else        tab[1].x = T1->P1.x + T1->speed * t; 

        t = (T1->P2.x - T2->P1.x)/(T1->speed - T2->speed);
        if(t<0)     tab[2].Collision = collisionNO;
        else        tab[2].x = T1->P2.x + T2->speed * t; 

        t = (T1->P2.x - T2->P2.x)/(T1->speed - T2->speed);
        if(t<0)     tab[3].Collision = collisionNO;
        else        tab[3].x = T1->P2.x + T2->speed * t; 
    }


    #if MODE_DEB == 1
    cout << endl;
    cout << tab[0].Collision << " | CPx: (" << tab[0].x << ")" << endl;
    cout << tab[1].Collision << " | CPx: (" << tab[1].x << ")" << endl;
    cout << tab[2].Collision << " | CPx: (" << tab[2].x << ")" << endl;
    cout << tab[3].Collision << " | CPx: (" << tab[3].x << ")" << endl;
    cout << endl;
    #endif // MODE_DE3


}



void DETERMINE_SpeedSignCorrection(MILLPEDE *T)
{
        if(T->P2.x < T->P1.x) T->speed *= -1; 
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
    cout << "Rotated P: " << MILL->CrossPoint.x << "," << MILL->CrossPoint.y << ")" << endl;
    cout << endl;


    if(MILL->lin.A >= 0)
    {
    MILL->CrossPoint.x = P.x - sT.Tx;
    MILL->CrossPoint.y = P.y - sT.Ty;
    }


    if(MILL->lin.A < 0)
    {
    MILL->CrossPoint.x = P.x + sT.Tx;
    MILL->CrossPoint.y = P.y + sT.Ty;
    }



    #if     MODE_DEB == 1
    cout << endl;
    cout << endl;
    cout << "        P: " << P.x << "," << P.y << endl;
    cout << "Shifted P: " <<MILL->CrossPoint.x << "," << MILL->CrossPoint.y << endl;
    #endif

    //
    // TODO    PRZY POWROCIE NAJPIERW ROTUJ PUZNIEJ PRZESUN
    //


    cout << "NAJPERW ROTUJ POZNIEJ PRZESUN" << endl;

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
    short jeden = 1;
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

	// linia pionowa
    //----------------| |-----------------|
    //        |       | | Expected LINFAC |
    //  P1    |  P2   | |  A  |  B  |  C  |
    //        |       | |     |     |     |
          0, 2,   0, 4,      1,    0,    0,
         10, 0,  10,10,      1,    0,  -10,
          0, 4,   0, 2,      1,    0,    0,
          3, 4,   3,-2,      1,    0,   -3,

	// linia pozioma
    //-----------------| |-----------------|
    //        |        | | Expected LINFAC |
    //   P1   |   P2   | |  A  |  B  |  C  |
    //        |        | |     |     |     |
        2, 0  ,  4, 0  ,      0,    1,    0,
        2,10  ,  4,10  ,      0,    1,  -10,
        2,-5  ,  4,-5  ,      0,    1,    5,

	// linia skosna
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

    // 0 = ST2A * X + ST2B + Y

    struct TESTTable
    {
    Point    S1P1;
    Point    S1P2;
    Point    S2P1;
    Point    S2P2;
    string   Expected_CrossPointExist;
    Point    Expected_CP;
    };

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine cross point TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    int AmountOfTest = 11;
    TESTTable TT[AmountOfTest] =
    {
    //----------------|---------------|------|--------|
    //ST1             |ST2            |CP    |CP      |
    //P1      |P2     |P1     |P2     |EXIST |Expect  |
    //        |       |       |       |      |        |
          0, 2,   0, 4,  2,  0,  4,  0, "YES",   0,   0,
         -2, 0,  -4, 0,  0, -2,  0, -4, "YES",   0,   0,
         -1, 1,  -2, 2,  1,  1,  2,  2, "YES",   0,   0,
         -1, 2,  -2, 3,  1,  2,  2,  3, "YES",   0,   1,
          0, 1,  -1, 1,  1,  0,  1, -1, "YES",   1,   1,

          1, 2,   0, 2,  2,  1,  2,  0, "YES",   2,   2,
          1, 2,  -1, 2,  2,  1,  2, -1, "YES",   2,   2,
         -4, 2,  -6, 2, -2,  0, -2, -2, "YES",  -2,   2,
         -2,-1,  -2, 1, -1, -2,  1, -2, "YES",  -2,  -2,

          0, 0,  -1, 1,  0,  1,  1,  2, "YES",-0.5, 0.5,
          0, 1,   1, 1,  0,  2,  1,  2, "NO" , 'X', 'X'
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
         ("YES" == TT[i].Expected_CrossPointExist)
         &&
         (fabs(MILL1.CrossPoint.x - TT[i].Expected_CP.x) <=1e-10)
         &&
         (fabs(MILL1.CrossPoint.y - TT[i].Expected_CP.y) <=1e-10)
        )
        {
            TEST_PASS;
        }
        else if
        ("NO" == TT[i].Expected_CrossPointExist)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout << " || ";
        cout << MILL1.CrossPointExist << "?>" << TT[i].Expected_CrossPointExist  << "<";
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
         CHECK_IF_CollisionPointbelongstoPlayZone(&MILL, &PlayZone)
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
        cout <<"  obs: " << CHECK_IF_CollisionPointbelongstoPlayZone(&MILL, &PlayZone);
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

    int AmountOfTest = 28;
    struct TESTTable
    {
        Point       P1; 
        Point       P2;
        Point       exp_P1;
        Point       exp_P2;
    };

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

    int AmountOfTest = 47;
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
        3,   -3,   -3,   -3.,    10, collisionNO,"brak przeciecia Trajektorie rownloegle poziome",

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
// TRAJEKTORIE PIONOWE NAKÅADAJÄ„CE SIÄ˜
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
        
       -1,-1      ,     -3,-3      ,   1   ,
        1,-1      ,      3,-3      ,   3   , collisionYES,"Case form the internet",
        
        1, 1      ,      3, 3      ,   1   ,
       -1, 1      ,     -3, 3      ,   3   , collisionYES,"Case form the internet",
        
        1, 1      ,     10,10      ,   1   ,
       -1, 1      ,    -10,10      ,  10   , collisionYES,"Case form the internet"  
    };


        //
        //      TODO
        //
        //      DUZO TESTOW Z LINIAMI ROWNOLEGLYMI
        //



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

#endif