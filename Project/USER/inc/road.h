#ifndef __ROAD_H
#define __ROAD_H

typedef enum
{
	forward,
	line_CBH,
	line,
	cross,
	
	Lisland,
	Risland,
	
	L_at_island,
	R_at_island,
	
	L_out_island,
	R_out_island,
	
	block_stage1,
	block_stage2,
	block_stage3,
	ramp_stage1,
	stop,
	left90,
	right90
} RoadItems;

typedef enum
{
	STATE_IDLE,
	STATE_ACTIVE,
	STATE_COMPLETE
} State;

void RoadItemsManager() reentrant;

//void processStateMachine();

void exitChargingMode(int run);
void finalStop();

void Follow();          // 普通巡线
void Follow_CBH();

void IslandFollow();

void Lget_in_island();   // 进入环岛
void Rget_in_island();

void Lat_island();
void Rat_island();

void Lget_out_island();
void Rget_out_island();

void Gostraight();

void Left90();
void Right90();

void Cross();
//void block1();//左偏
//void block2();//右转
//void block3();//捕获

#endif

