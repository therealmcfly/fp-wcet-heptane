/* This file has been generated from conf/flight_plans/braunschweig.xml */
/* Please DO NOT EDIT */

#ifndef FLIGHT_PLAN_H
#define FLIGHT_PLAN_H 

#define FLIGHT_PLAN_NAME "EMAV 2004, 8 shape"
#define NAV_UTM_EAST0 605530
#define NAV_UTM_NORTH0 5797350
#define QFU 270.0
#define WP_HOME 0
#define WAYPOINTS { \
 {0.0, 0.0, 200},\
 {0.0, 0.0, 200},\
 {115.0, -75.0, 200},\
 {156.7, -41.7, 200},\
 {115.0, 0.0, 200},\
 {0.0, -75.0, 200},\
 {-51.7, -36.7, 200},\
};
#define NB_WAYPOINT 7
#define GROUND_ALT 125.
#define SECURITY_ALT 150.
#define MAX_DIST_FROM_HOME 500.
#ifdef NAV_C

static inline void auto_nav(void) {
// The switch-less version of the code is located beyond. - BL
#if 0
  switch (nav_block) {
    Block(0) // init
    switch(nav_stage) {
      Label(while_1)
      Stage(0)
        if (! (!(estimator_flight_time))) Goto(endwhile_2) else NextStage();
        Stage(1)
          Goto(while_1)
        Label(endwhile_2)
      Stage(2)
        if ((estimator_flight_time>8)) NextStage() else {
          desired_course = RadOfDeg(QFU);
          auto_pitch = FALSE;
          nav_pitch = 0.150000;
          vertical_mode = VERTICAL_MODE_AUTO_GAZ;
          nav_desired_gaz = TRIM_UPPRZ(0.800000*MAX_PPRZ);
        }
        return;
      Stage(3)
        if ((estimator_z>SECURITY_ALT)) NextStage() else {
          desired_course = RadOfDeg(QFU);
          auto_pitch = FALSE;
          nav_pitch = 0.000000;
          vertical_mode = VERTICAL_MODE_AUTO_CLIMB;
          desired_climb = 8.000000;
        }
        return;
      Stage(4)
        NextBlock()
    }

    Block(1) // two
    if RcEvent1() { GotoBlock(2) }
    switch(nav_stage) {
      Label(while_3)
      Stage(0)
        if (! (TRUE)) Goto(endwhile_4) else NextStage();
        Stage(1)
          if (approaching(1)) NextStageFrom(1) else {
            fly_to(1);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[1].a;
            pre_climb = 0.;
          }
          return;
        Stage(2)
          if (approaching(4)) NextStageFrom(4) else {
            fly_to(4);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[4].a;
            pre_climb = 0.;
          }
          return;
        Stage(3)
          Goto(while_3)
        Label(endwhile_4)
      Stage(4)
        NextBlock()
    }

    Block(2) // height
    if RcEvent1() { GotoBlock(3) }
    switch(nav_stage) {
      Label(while_5)
      Stage(0)
        if (! (TRUE)) Goto(endwhile_6) else NextStage();
        Stage(1)
          if (approaching(6)) NextStageFrom(6) else {
            fly_to(6);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[6].a;
            pre_climb = 0.;
          }
          return;
        Stage(2)
          if (approaching(1)) NextStageFrom(1) else {
            fly_to(1);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[1].a;
            pre_climb = 0.;
          }
          return;
        Stage(3)
          if (approaching(2)) NextStageFrom(2) else {
            route_to(last_wp, 2);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[2].a;
            pre_climb = 0.;
          }
          return;
        Stage(4)
          if (approaching(3)) NextStageFrom(3) else {
            fly_to(3);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[3].a;
            pre_climb = 0.;
          }
          return;
        Stage(5)
          if (approaching(4)) NextStageFrom(4) else {
            fly_to(4);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[4].a;
            pre_climb = 0.;
          }
          return;
        Stage(6)
          if (approaching(5)) NextStageFrom(5) else {
            route_to(last_wp, 5);
            auto_pitch = FALSE;
            nav_pitch = 0.000000;
            vertical_mode = VERTICAL_MODE_AUTO_ALT;
            desired_altitude = waypoints[5].a;
            pre_climb = 0.;
          }
          return;
        Stage(7)
          Goto(while_5)
        Label(endwhile_6)
      Stage(8)
        NextBlock()
    }

    Block(3) // xyz
    if RcEvent1() { GotoBlock(4) }
    switch(nav_stage) {
      Stage(0)
        Goto3D(50)
        return;
      Stage(1)
        NextBlock()
    }

    Block(4) // circle
    if RcEvent1() { GotoBlock(5) }
    switch(nav_stage) {
      Stage(0)
        auto_pitch = FALSE;
        nav_pitch = 0.000000;
        vertical_mode = VERTICAL_MODE_AUTO_ALT;
        desired_altitude = waypoints[0].a;
        pre_climb = 0.;
        Circle(0, 150);
        return;
      Stage(1)
        NextBlock()
    }

    Block(5) // hippo
    if RcEvent1() { GotoBlock(1) }
    switch(nav_stage) {
      Label(while_7)
      Stage(0)
        if (! (TRUE)) Goto(endwhile_8) else NextStage();
        Stage(1)
          auto_pitch = FALSE;
          nav_pitch = 0.000000;
          vertical_mode = VERTICAL_MODE_AUTO_ALT;
          desired_altitude = waypoints[1].a;
          pre_climb = 0.;
          Circle(1, 100);
          if (Qdr(0)) NextStage();
          return;
        Stage(2)
          auto_pitch = FALSE;
          nav_pitch = 0.000000;
          vertical_mode = VERTICAL_MODE_AUTO_ALT;
          desired_altitude = waypoints[4].a;
          pre_climb = 0.;
          Circle(4, 100);
          if (Qdr(180)) NextStage();
          return;
        Stage(3)
          Goto(while_7)
        Label(endwhile_8)
      Stage(4)
        NextBlock()
    }

  }
#endif


	     if (nav_block == 0) {Goto(nav_block_case_0)}
	else if (nav_block == 1) {Goto(nav_block_case_1)}
	else if (nav_block == 2) {Goto(nav_block_case_2)}
	else if (nav_block == 3) {Goto(nav_block_case_3)}
	else if (nav_block == 4) {Goto(nav_block_case_4)}
	else if (nav_block == 5) {Goto(nav_block_case_5)}
	else                     {Goto(nav_block_default)}
	

	Label(nav_block_case_0); nav_block = 0; // init
		/* 	 Switch nav_stage                             */
		     if (nav_stage == 0) {Goto(nav_0_stage_0)}
		else if (nav_stage == 1) {Goto(nav_0_stage_1)}
		else if (nav_stage == 2) {Goto(nav_0_stage_2)}
		else if (nav_stage == 3) {Goto(nav_0_stage_3)}
		else if (nav_stage == 4) {Goto(nav_0_stage_4)}
		else                     {Goto(nav_0_stage_default)}

		Label(while_1)
		/* case nav_stage 0 */
		Label(nav_0_stage_0); nav_stage = 0;
			if (! (!(estimator_flight_time))) Goto(endwhile_2) else NextStage();
		/* case nav_stage 1 */
		Label(nav_0_stage_1); nav_stage = 1;
			Goto(while_1)
		Label(endwhile_2)
		/* case nav_stage 2 */
		Label(nav_0_stage_2); nav_stage = 2;
			if ((estimator_flight_time>8)) NextStage() else {
				desired_course = RadOfDeg(QFU);
				auto_pitch = FALSE;
				nav_pitch = 0.150000;
				vertical_mode = VERTICAL_MODE_AUTO_GAZ;
				nav_desired_gaz = TRIM_UPPRZ(0.800000*MAX_PPRZ);
			}
			return;
		/* case nav_stage 3 */
		Label(nav_0_stage_3); nav_stage = 3;
			if ((estimator_z>SECURITY_ALT)) NextStage() else {
				desired_course = RadOfDeg(QFU);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_CLIMB;
				desired_climb = 8.000000;
			}
			return;
		/* case nav_stage 4 */
		Label(nav_0_stage_4); nav_stage = 4;
			NextBlock()

		Label(nav_0_stage_default)
		/* End switch nav_stage */
	

	Label(nav_block_case_1); nav_block = 1; // two
    if RcEvent1() { GotoBlock(2) }

		/* 	 switch nav_stage                             */
		     if (nav_stage == 0) {Goto(nav_1_stage_0)}
		else if (nav_stage == 1) {Goto(nav_1_stage_1)}
		else if (nav_stage == 2) {Goto(nav_1_stage_2)}
		else if (nav_stage == 3) {Goto(nav_1_stage_3)}
		else if (nav_stage == 4) {Goto(nav_1_stage_4)}
		else                     {Goto(nav_1_stage_default)}
		

		Label(while_3)
		/* case nav_stage 0 */
		Label(nav_1_stage_0); nav_stage = 0;
			if (! (TRUE)) Goto(endwhile_4) else NextStage();
		/* case nav_stage 1 */
		Label(nav_1_stage_1); nav_stage = 1;
			if (approaching(1)) NextStageFrom(1) else {
				fly_to(1);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[1].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 2 */
		Label(nav_1_stage_2); nav_stage = 2;
			if (approaching(4)) NextStageFrom(4) else {
				fly_to(4);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[4].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 3 */
		Label(nav_1_stage_3); nav_stage = 3;
			Goto(while_3)
		Label(endwhile_4)
		/* case nav_stage 4 */
		Label(nav_1_stage_4); nav_stage = 4;
			NextBlock()

		Label(nav_1_stage_default)
		/*End switch nav_stage*/

	/* case nav_block 2 */
	Label(nav_block_case_2); nav_block = 2; // height
    if RcEvent1() { GotoBlock(3) }
	
		/*   switch nav_stage                             */
		     if (nav_stage == 0) {Goto(nav_2_stage_0)}
		else if (nav_stage == 1) {Goto(nav_2_stage_1)}
		else if (nav_stage == 2) {Goto(nav_2_stage_2)}
		else if (nav_stage == 3) {Goto(nav_2_stage_3)}
		else if (nav_stage == 4) {Goto(nav_2_stage_4)}
		else if (nav_stage == 5) {Goto(nav_2_stage_5)}
		else if (nav_stage == 6) {Goto(nav_2_stage_6)}
		else if (nav_stage == 7) {Goto(nav_2_stage_7)}
		else if (nav_stage == 8) {Goto(nav_2_stage_8)}
		else                     {Goto(nav_2_stage_default)}

		Label(while_5)
		/* case nav_stage 0 */
		Label(nav_2_stage_0); nav_stage = 0;
			if (! (TRUE)) Goto(endwhile_6) else NextStage();
		/* case nav_stage 1 */
		Label(nav_2_stage_1); nav_stage = 1;
			if (approaching(6)) NextStageFrom(6) else {
				fly_to(6);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[6].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 2 */
		Label(nav_2_stage_2); nav_stage = 2;
			if (approaching(1)) NextStageFrom(1) else {
				fly_to(1);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[1].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 3 */
		Label(nav_2_stage_3); nav_stage = 3;
			if (approaching(2)) NextStageFrom(2) else {
				route_to(last_wp, 2);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[2].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 4 */
		Label(nav_2_stage_4); nav_stage = 4;
			if (approaching(3)) NextStageFrom(3) else {
				fly_to(3);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[3].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 5 */
		Label(nav_2_stage_5); nav_stage = 5;
			if (approaching(4)) NextStageFrom(4) else {
				fly_to(4);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[4].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 6 */
		Label(nav_2_stage_6); nav_stage = 6;
			if (approaching(5)) NextStageFrom(5) else {
				route_to(last_wp, 5);
				auto_pitch = FALSE;
				nav_pitch = 0.000000;
				vertical_mode = VERTICAL_MODE_AUTO_ALT;
				desired_altitude = waypoints[5].a;
				pre_climb = 0.;
			}
			return;
		/* case nav_stage 7 */
		Label(nav_2_stage_7); nav_stage = 7;
			Goto(while_5)
		Label(endwhile_6)
		/* case nav_stage 8 */
		Label(nav_2_stage_8); nav_stage = 8;
			NextBlock()

		Label(nav_2_stage_default)
		/* End switch nav_stage */

	Label(nav_block_case_3); nav_block = 3; // xyz
    if RcEvent1() { GotoBlock(4) }

		/*   switch nav_stage                             */
         if (nav_stage == 0) {Goto(nav_3_stage_0)}
		else if (nav_stage == 1) {Goto(nav_3_stage_1)}
		else                     {Goto(nav_3_stage_default)}

		/* case nav_stage 0 */
		Label(nav_3_stage_0); nav_stage = 0;
			Goto3D(50)
			return;
		/* case nav_stage 1 */
		Label(nav_3_stage_1); nav_stage = 1;
			NextBlock()

		Label(nav_3_stage_default)
		/* End switch nav_stage */

	Label(nav_block_case_4); nav_block = 4; // circle
		if RcEvent1() { GotoBlock(5) }

		/*   switch nav_stage                             */
         if (nav_stage == 0) {Goto(nav_4_stage_0)}
		else if (nav_stage == 1) {Goto(nav_4_stage_1)}
		else                     {Goto(nav_4_stage_default)}

		/* case nav_stage 0 */
		Label(nav_4_stage_0); nav_stage = 0;
			auto_pitch = FALSE;
			nav_pitch = 0.000000;
			vertical_mode = VERTICAL_MODE_AUTO_ALT;
			desired_altitude = waypoints[0].a;
			pre_climb = 0.;
			Circle(0, 150);
			return;
		/* case nav_stage 1 */
		Label(nav_4_stage_1); nav_stage = 1;
			NextBlock()

		Label(nav_4_stage_default)
		/* End switch nav_stage */

	Label(nav_block_case_5); nav_block = 5; // hippo
    if RcEvent1() { GotoBlock(1) }

		/*   switch nav_stage                             */
         if (nav_stage == 0) {Goto(nav_5_stage_0)}
		else if (nav_stage == 1) {Goto(nav_5_stage_1)}
		else if (nav_stage == 2) {Goto(nav_5_stage_2)}
		else if (nav_stage == 3) {Goto(nav_5_stage_3)}
		else if (nav_stage == 4) {Goto(nav_5_stage_4)}
		else                     {Goto(nav_5_stage_default)}

		Label(while_7)
		/* case nav_stage 0 */
		Label(nav_5_stage_0); nav_stage = 0;
			if (! (TRUE)) Goto(endwhile_8) else NextStage();
		/* case nav_stage 1 */
		Label(nav_5_stage_1); nav_stage = 1;
			auto_pitch = FALSE;
			nav_pitch = 0.000000;
			vertical_mode = VERTICAL_MODE_AUTO_ALT;
			desired_altitude = waypoints[1].a;
			pre_climb = 0.;
			Circle(1, 100);
			if (Qdr(0)) NextStage();
			return;
		/* case nav_stage 2 */
		Label(nav_5_stage_2); nav_stage = 2;
			auto_pitch = FALSE;
			nav_pitch = 0.000000;
			vertical_mode = VERTICAL_MODE_AUTO_ALT;
			desired_altitude = waypoints[4].a;
			pre_climb = 0.;
			Circle(4, 100);
			if (Qdr(180)) NextStage();
			return;
		/* case nav_stage 3 */
		Label(nav_5_stage_3); nav_stage = 3;
			Goto(while_7)
		Label(endwhile_8)
		/* case nav_stage 4 */
		Label(nav_5_stage_4); nav_stage = 4;
			NextBlock()
		;
		Label(nav_5_stage_default)
		/* End switch nav_stage */

	Label(nav_block_default)
	;

}
#endif // NAV_C

#endif // FLIGHT_PLAN_H
