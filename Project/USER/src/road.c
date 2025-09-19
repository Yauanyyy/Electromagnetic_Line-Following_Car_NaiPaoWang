#include "headfile.h"


/**
 * @brief  使小车退出充电模式并开始一段直行
 * @param  run: int, 设定小车向前行驶的距离 (单位: 编码器脉冲数)。
 * @return void
 * @note   该函数用于比赛开始或手动触发，将小车从静止状态切换到正常的循迹状态。
 * @author yanyyy
 * @date   2025-09-19
 */
void exitChargingMode(int run)
{
	currItem = forward;
	setSpeed = normalSpeed;
	distance_set = run;
}


/**
 * @brief  赛道元素管理器，根据传感器数据判断并切换车辆状态
 * @param  None
 * @return void
 * @note   这是一个核心的状态机函数，被标记为 `reentrant` (可重入)。
 *         它负责识别如十字、环岛等赛道元素，并设置相应的动作序列
 *         (如直行距离 `distance_set` 或转弯角度 `angle_set`)。
 *         环岛处理是多阶段的，由 `get_in_island_FLAG` 标志位控制。
 * @author yanyyy
 * @date   2025-09-19
 */
void RoadItemsManager() reentrant
{
    // --- 默认状态: 正常巡线 ---
    // 当没有特定任务(直行、转弯、入环)时，默认状态为巡线
	if ( (distance_set == 0) && (angle_set == 0) && (get_in_island_FLAG == 0))
	{
		currItem = line;
		P67 = 0;
	}

	
	// --- 元素检测: 十字路口 ---
    // 左右竖直电感同时检测到强信号，且中间电感在特定范围，判断为十字
	if ( ( (adc_guiyi[2] > 38) && (adc_guiyi[3] > 38)
		//&& (adc_guiyi[1] < 35) && (adc_guiyi[4] < 35)
		&&(adc_guiyi[5] < 70) && (adc_guiyi[5] > 30)
		&& (get_in_island_FLAG == 0) && (distance_set == 0) && (angle_set == 0)))
	{
		currItem = cross;
		distance_set = 6000;//2025 0719 2041: 3000
		P67 = 1;
	}

	// --- 元素检测: 左入环岛 (多阶段) ---
    // 阶段 1: 检测到入环点 (中间电感信号强，且左侧电感强于右侧)
	else if ( (adc_guiyi[5] > 76) && ( adc_guiyi[1] > adc_guiyi[4]) && (adc_guiyi[2] > adc_guiyi[3])
	          && (get_in_island_FLAG == 0) && (distance_set == 0) && (angle_set == 0))
	{
		get_in_island_FLAG = 1;
		currItem = Lisland;
		//distance_set = 2000;
		angle_set = 6111;
		P67 = 1;
	}
//	else if ((adc_guiyi[4] > 98) && (adc_guiyi[1] > 98)
//			&& (get_in_island_FLAG == 1) && (distance_set == 0) && (angle_set == 0))
//	{
//			get_in_island_FLAG = 2;
//			currItem = Lisland;
//			angle_set = 12000;//9333
//			distance_set = 1000;
//	}
	// 阶段 2: 转弯完成 (通过累计角度判断)，切换到直行状态以调整姿态
	else if ((get_in_island_FLAG == 1) && (distance_set == 0) && (angle_accu > 800))
	{
		get_in_island_FLAG = 2;
		currItem = line;
	}
	// 阶段 3: 准备出环，在环岛内直行一段距离
	else if ((get_in_island_FLAG == 2) && (distance_set == 0) && (angle_set == 0))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
	{
		get_in_island_FLAG = 3;
		currItem = line;
		distance_set = 10000;
		P67 = 0;
	}
	// 阶段 4: 出环完成，重置环岛标志位
	else if ((get_in_island_FLAG == 3) && (angle_set == 0)&& (distance_set == 0))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
	{
		get_in_island_FLAG = 0;
	}
	
	// --- 元素检测: 右入环岛 (多阶段) ---
    // 阶段 1: 检测到入环点 (中间电感信号强，且右侧电感强于左侧)
	else if ( (adc_guiyi[5] > 85) && ( adc_guiyi[4] > adc_guiyi[1]) && (adc_guiyi[3] > adc_guiyi[2])
	          && (get_in_island_FLAG == 0) && (distance_set == 0) && (angle_set == 0))
	{
		get_in_island_FLAG = 11;
		currItem = Risland;
		//distance_set = 2000;
		angle_set = -6500;
		P67 = 1;
	}
	// 阶段 2: 转弯完成，切换到直行
	else if ((get_in_island_FLAG == 11) && (distance_set == 0) && (angle_accu < -800))
	{
		get_in_island_FLAG = 22;
		currItem = line;
	}
	// 阶段 3: 准备出环，直行
	else if ((get_in_island_FLAG == 22) && (distance_set == 0) && (angle_set == 0))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
	{
		get_in_island_FLAG = 33;
		currItem = line;
		distance_set = 10000;
		P67 = 0;
	}
	// 阶段 4: 出环完成，重置标志位
	else if ((get_in_island_FLAG == 33) && (angle_set == 0)&& (distance_set == 0))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
	{
		get_in_island_FLAG = 0;
	}	
	
	//	else if ((get_in_island_FLAG == 5) && (distance_set == 0))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
//	{
//		get_in_island_FLAG = 0;
//		currItem = line;
//		P67 = 0;
//	}
//	
	
	
	

//	if ( ( (adc_guiyi[4] - adc_guiyi[1]) > 10) && ( (adc_guiyi[3] + adc_guiyi[2]) > 17000) //Right
//	        && ( (adc_guiyi[5] - adc_guiyi[0]) > 35) && (adc_guiyi[0] > 5)
//	        && (get_in_island_FLAG == 0) && (distance_set == 0) && (angle_set == 0))
//	{
//		get_in_island_FLAG = 11;
//		currItem = Risland;
//		//angle_set = 18000;
//		distance_set = 5000;
//		//P67 = 1;
//	}

//	else if ( ( (adc_guiyi[1] - adc_guiyi[4]) > 25) && ( (adc_guiyi[3] + adc_guiyi[2]) < 30) //Left
//	          //&& ( (adc_guiyi[0] - adc_guiyi[5]) > 50)
//	          && (get_in_island_FLAG == 0) && (distance_set == 0) && (angle_set == 0))
//	{
//		get_in_island_FLAG = 1;
//		currItem = forward;
//		angle_set = 12500;
//		distance_set = 1000;
//		P67 = 1;
//	}

//	else if ((get_in_island_FLAG == 1) && (distance_set == 0) && (adc_guiyi[1] > 95) )
//	{
//		get_in_island_FLAG = 2;
//		currItem = Lisland;
//		P67 = 0;
//	}
//	else if ((get_in_island_FLAG == 2) && (distance_set == 0) && (angle_set == 0))
//	{
//		get_in_island_FLAG = 3;
//		distance_set = 2000;
//		currItem = line_CBH;
//		//P67 = 0;
//	}
//	else if ((get_in_island_FLAG == 3) && (distance_set == 0) && (adc_guiyi[5] > 60))// && (adc_guiyi[0] > 15) && (adc_guiyi[5] > 60)
//	{
//		get_in_island_FLAG = 0;
//		currItem = L_out_island;
//		distance_set = 5000;
//		//P67 = 1;
//	}
//

}

void Follow()
{
	setSpeed = normalSpeed;
	fix = PID_ADC();
	//P67 = 0;
}



void Follow_CBH()
{
	setSpeed = normalSpeed;
	fix = PID_ADC_2();
	//P67 = 0;
}

void Cross()
{
	setSpeed = botSpeed;
	fix = 0;
}
void Left90()
{
	setSpeed = normalSpeed;
	fix = PID_ADC() * 1.3;
}

void Right90()
{
	setSpeed = normalSpeed;
	fix = PID_ADC() * 1.3;
}


void Lget_in_island() //左偏入环
{
	setSpeed = normalSpeed;
	fix = -11;

}

void Rget_in_island()
{
	setSpeed = normalSpeed;
	fix = 12.0;
}

void Lat_island()
{
	setSpeed = normalSpeed;
	fix = -6.25;
}

void Rat_island()
{
	setSpeed = normalSpeed;
	fix = 3.75;//2.0
}

void Lget_out_island()
{
	setSpeed = normalSpeed;
	fix = -3.0;
}

void Rget_out_island()
{
	setSpeed = normalSpeed;
	fix = 3.0;
}

void finalStop()
{
	setSpeed = 0;
	fix = 0;
}
void Gostraight()
{
	setSpeed = normalSpeed;
	fix = 0;
}


//void block1()
//{ // 左偏
//    setSpeed = 40;
//    fix = -950/50;
//}
//void block2()
//{ // 右转
//    setSpeed = 40;
//    fix = 450/50;
//}
//void block3()
//{ // 捕获
//    setSpeed = 40;
//    fix = -1000/50;
//}







//void RoadItemsManager()reentrant// 根据当前电感值和编码器数据判断赛道类型，编码器1cm计数100，车旋转90°左右编码器相差1300
//{
////	int a,b,c,d,e,f;
////	a = adc_guiyi[0]; b = adc_guiyi[5];
////	c = adc_guiyi[1]; d = adc_guiyi[4];
////	e = adc_guiyi[2]; f = adc_guiyi[3];
////	//防止进岛前提前变线
////	if(((d-c) > 10) && ((f-e) < 20) && ((b-a) > 50) &&(a > 5) &&  before_get_in_island ==0)
//////	if((adc_guiyi[2] + adc_guiyi[3] > 100) && before_get_in_island ==0 )
////	{
////		before_get_in_island = 1;
////		currItem = forward;
////		distance_set = 2000;
////	}
////
////	if(((c-d) > 5) && ((e-f) < 10) &&  ((a-b) > 80) && before_get_in_island ==0)
//////	if((adc_guiyi[2] + adc_guiyi[3] > 100) && before_get_in_island ==0 )
////	{
////		before_get_in_island = 1;
////		currItem = forward;
////		distance_set = 1000;
////	}
////
////	if (before_get_in_island == 1)
////	{
////		if (distance_set == 0)
////		{
////			before_get_in_island = 0;
////			currItem = line;
////		}
////	}

//	//直角弯







//    // 圆环部分
////    if ((adc_sum > 350) && (adc_guiyi[0] < 60) && (adc_guiyi[0] > 40) && (adc_guiyi[5] > 90) && (adc_guiyi[1] + adc_guiyi[4] < 70) && (get_in_island_FLAG == 0))////////
//	if ((adc_sum > 280) && (adc_guiyi[0] > 90) && (adc_guiyi[5] < 30) && (adc_guiyi[2] + adc_guiyi[3] > 150) && (adc_guiyi[1] + adc_guiyi[4] < 70) && (get_in_island_FLAG == 0) && (distance_set == 0))
//    {
//        if (get_in_island_FLAG == 0)
//        {
//            get_in_island_FLAG = 1;
//            currItem = Lisland;
//            distance_set = 6000;////////
//						P67 = 1;
//        }
//    }
////    if ((adc_sum > 350) && (adc_guiyi[5] < 60) && (adc_guiyi[5] > 40) && (adc_guiyi[0] > 90) && (adc_guiyi[1] + adc_guiyi[4] < 70) && (get_in_island_FLAG == 0))////////
//	if ((adc_sum > 280) && (adc_guiyi[5] > 65) && (adc_guiyi[0] < 50) && (adc_guiyi[2] + adc_guiyi[3] > 150) && (adc_guiyi[1] + adc_guiyi[4] < 70) && (get_in_island_FLAG == 0) && (distance_set == 0))
//    {
//        if (get_in_island_FLAG == 0)
//        {
//            get_in_island_FLAG = 1;
//            currItem = Risland;
//            distance_set = 6000;////////
//						P67 = 1;
//        }
//    }
//
////    if (get_in_island_FLAG == 1)
////    {
////        if (distance_set == 0)
////        {
////            if (jumpIsland == 1)
////            {
////                get_in_island_FLAG = 0;
////                currItem = line;
////            }
////            else
////            {
////                get_in_island_FLAG = 2;
////                currItem = line;
////								P67 = 0;
////            }
////        }
////    }
//    if ((get_in_island_FLAG == 1))
//    {
//      if (distance_set == 0)
//			{
//////			&& adc_guiyi[0] > 150))////////
////        {
////
////            currItem = outisland1;
////            circle_direc_FLAG = 1;
////            distance_set = outIslandTurn;
////        }
////        else if ((adc_guiyi[0] < 100))
//////			&& adc_guiyi[1] > 150))////////
////        {
////            currItem = outisland1;
////            circle_direc_FLAG = 2;
////            distance_set = outIslandTurn;
////        }
//			  currItem = line;
//				distance_set = 6000;
//				get_in_island_FLAG = 2;
//			}
//    }
//
//		if (get_in_island_FLAG == 2)
//		{
//			if((distance_set == 0) && ((adc_guiyi[0] > 85)||(adc_guiyi[5] > 65)) && (adc_guiyi[2] + adc_guiyi[3] > 150))
//			{
//				currItem = outisland2;
//				distance_set = 4000;
//				get_in_island_FLAG = 3;
//			}
//		}
//		if (get_in_island_FLAG == 3)
//		{
//			if(distance_set == 0)
//			{
//				currItem = line;
//				distance_set = 10000;
//				get_in_island_FLAG = 4;
//			}
//		}
//		if (get_in_island_FLAG == 4)
//		{
//			if(distance_set == 0)
//			{
//				currItem = line;
//				get_in_island_FLAG = 0;
//			}
//		}
////    if (currItem == outisland1)
////    {
////        if (distance_set == 0)
////        {
////            currItem = outisland2;
////            distance_set = outIslandStraight;
////        }
////    }
////    if (currItem == outisland2)
////    {
////        if (distance_set == 0)
////        {
////            currItem = line;
////            get_in_island_FLAG = 0;
////        }
////    }
//
//	/*
//	// 避障部分
//    if ((dl1a_distance_mm > 5000) && (block_FLAG == 1) && (adc_sum > 50) && (currItem != block_stage1) && (currItem != block_stage2) && (currItem != block_stage3))
//    {
//        block_FLAG = 0;
//    }
//    if ((dl1a_distance_mm < 1000) && (dl1a_distance_mm > 700) && (adc_sum > 50))
//    {
//        if (distance_set == 0)
//        {
//            block_FLAG = 1;
//        }
//    }
//    if ((dl1a_distance_mm < 650) && (block_FLAG == 1) && (adc_sum > 50))
//    {
//        if (distance_set == 0)
//        {
//            currItem = block_stage1;
//            distance_set = blockLeft; // 400ms左偏
//        }
//    }
//    else if (currItem == block_stage1)
//    {
//        if (distance_set == 0)
//        {
//            currItem = block_stage2;
//            block_FLAG = 1;
//            distance_set = blockRight; // 800ms右偏
//        }
//    }
//    else if (currItem == block_stage2)
//    {
//        if ((distance_set == 0))
//        {
//            currItem = block_stage3;
//					distance_set = blockBack;
//        }
//    }
//    else if (currItem == block_stage3)
//    {
//        if ((adc_sum > 50) && (distance_set == 0))
//        {
//            block_FLAG = 0;
//            currItem = line;
//        }
//    }
//
//	*/
//
//
////	//无法区分坡道和障碍时
////    if ((dl1a_distance_mm > 5000) && (block_FLAG == 1))
////    {
////        block_FLAG = 0;
////    }
////    if ((dl1a_distance_mm < 1000) && (dl1a_distance_mm > 700) && (adc_sum > 30) && (order_FLAG = 1))
////    {
////        if (distance_set == 0)
////        {
////            block_FLAG = 1;
////        }
////    }
////    if ((dl1a_distance_mm < 800) && (block_FLAG == 1) && (adc_sum > 50))
////    {
////        if (distance_set == 0)
////        {
////            currItem = block_stage1;
////            distance_set = blockLeft; // 400ms左偏
////        }
////    }
////    else if (currItem == block_stage1)
////    {
////        if (distance_set == 0)
////        {
////            currItem = block_stage2;
////            block_FLAG = 1;
////            distance_set = blockRight; // 800ms右偏
////        }
////    }
////    else if (currItem == block_stage2)
////    {
////        if ((distance_set == 0))
////        {
////            currItem = block_stage3;
////						distance_set = 3000;
////        }
////    }
////    else if (currItem == block_stage3)
////    {
////        if ((distance_set == 0) && (adc_sum > 0))
////        {
////            block_FLAG = 0;
////            currItem = line;
////        }
////    }
////	//坡道部分
////	if ((dl1a_distance_mm > 5000) && (ramp_FLAG == 1))
////    {
////        ramp_FLAG = 0;
////    }
////	if ((dl1a_distance_mm < 900) && (dl1a_distance_mm > 600) && (adc_sum > 50) && (order_FLAG = 2))
////    {
////        if (distance_set == 0)
////        {
////            ramp_FLAG = 1;
////        }
////    }
////	if ((dl1a_distance_mm < 550) && (ramp_FLAG == 1) && (adc_sum > 30))
////    {
////        if (distance_set == 0)
////        {
////            currItem = ramp_stage1;
////            distance_set = rampset;
////        }
////    }
////    else if (currItem == ramp_stage1)
////    {
////        if (distance_set == 0)
////        {
////            currItem = line;
////            ramp_FLAG = 0;
////			order_FLAG = 1;
////        }
////    }


//	if((adc_guiyi[1] - adc_guiyi[4] > 60) && (adc_guiyi[2] + adc_guiyi[3] < 110))
//	{
//		currItem = left90;
//
//	}
//
//		if((adc_guiyi[4] - adc_guiyi[1] > 60) && (adc_guiyi[2] + adc_guiyi[3] < 110))
//	{
//		currItem = right90;
//
//	}
//}


//void RoadItemsManager() reentrant // 根据当前电感值和编码器数据判断赛道
//{

//	//                0  1  2       3  4  5
//	currItem = line;
//	// 圆环部分
//	if ( (adc_guiyi[0] > 42) && (get_in_island_FLAG == 0)
//		&& (adc_guiyi[2]+ adc_guiyi[3] > 198) && (adc_guiyi[1]+ adc_guiyi[4] < 70)
//		&& (distance_set == 0) )
//
//	{
//		get_in_island_FLAG = 2;
//		currItem = L_at_island;
//		distance_set = 10000;//10000走小环
//		P67 = 1;
//	}
//	else if ( (adc_guiyi[5] > 36) && (get_in_island_FLAG == 0)
//		&& (adc_guiyi[2]+ adc_guiyi[3] > 198) && (adc_guiyi[1]+ adc_guiyi[4] < 90)
//		&& (distance_set == 0) )
//	{
//		get_in_island_FLAG = 20;
//		currItem = R_at_island;
//		distance_set = 10000;
//		P67 = 1;
//	}
//
//
//
//	else if ( (get_in_island_FLAG == 2) && (distance_set == 0) )
//	{
//		currItem = line;
//		distance_set = 50000;//50000走到出小圆环一点
//		get_in_island_FLAG = 3;//3
//		P67 = 0;
//	}
//
//	else if ( (get_in_island_FLAG == 20) && (distance_set == 0) )
//	{
//		currItem = line;
//		distance_set = 50000;
//		get_in_island_FLAG = 30;//30
//		P67 = 0;
//	}
//
//
//	else if ( (get_in_island_FLAG == 3) && (distance_set == 0) )
//	{
//		P67 = 0;
//		get_in_island_FLAG = 0;
//	}
//
//	else if ( (get_in_island_FLAG == 30) && (distance_set == 0) )
//	{
//		P67 = 0;
//		get_in_island_FLAG = 0;
//	}

//	//十字
//	else if ( ( (adc_guiyi[1] + adc_guiyi[4]) > 150) && (get_in_island_FLAG == 0) && (distance_set == 0)) // 冲十字////////
//	{
//		currItem = cross;
//		distance_set = 1500;
//		//P67 = 1;
//	}
//
//
//	/*
//
//
//	if ( (get_in_island_FLAG == 3) &&  (distance_set == 0) )
//	{
//		if ( adc_guiyi[0] + adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] > 430)
//		{
//			currItem = L_out_island;//走直线
//			distance_set = 8000;
//			get_in_island_FLAG = 4;
//		}
//
//	}
//	if ( (get_in_island_FLAG == 30) &&  (distance_set == 0) )
//	{
//		if ( adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] + adc_guiyi[5] > 430)
//		{
//			currItem = R_out_island;//走直线
//			distance_set = 8000;
//			get_in_island_FLAG = 4;
//		}
//
//	}
//
//
//	if (get_in_island_FLAG == 4)
//	{
//		if (distance_set == 0)
//		{
//			currItem = line;
//			distance_set = 5000;
//			get_in_island_FLAG = 0;
//

//		}
//	}
//	*/
//
//
//
//
//
//
////	if (get_in_island_FLAG == 4)
////	{
////		if (distance_set == 0)
////		{
////			currItem = line;
////			get_in_island_FLAG = 0;
////		}

////	}



//	else if((adc_guiyi[1] - adc_guiyi[4] > 50) && (adc_guiyi[2] + adc_guiyi[3] < 130))
//	{
//		currItem = left90;

//	}

//	else if((adc_guiyi[4] - adc_guiyi[1] > 50) && (adc_guiyi[2] + adc_guiyi[3] < 130))
//	{
//		currItem = right90;

//	}



//}





////V2:
//void RoadItemsManager() reentrant // 根据当前电感值和编码器数据判断赛道
//{

//	//                0  1  2       3  4  5
//	if ( (distance_set == 0) && (angle_set == 0))
//	{
//		currItem = line;
//	}

//	//防止进岛前提前变线
//	if ( ( (adc_guiyi[4] - adc_guiyi[1]) > 10) && ( (adc_guiyi[3] + adc_guiyi[2]) > 198) //Right
//	        && ( (adc_guiyi[5] - adc_guiyi[0]) > 35) && (adc_guiyi[0] > 5)
//	        && (before_get_in_island == 0) && (distance_set == 0) && (angle_set == 0))
//	{
//		before_get_in_island = 1;
//		currItem = forward;
//		distance_set = 5000;
//		//P67 = 1;
//	}

//	else if ( ( (adc_guiyi[1] - adc_guiyi[4]) > 10) && ( (adc_guiyi[3] + adc_guiyi[2]) > 198) //Left
//	          && ( (adc_guiyi[0] - adc_guiyi[5]) > 35)
//	          && (before_get_in_island == 0) && (distance_set == 0) && (angle_set == 0))
//	{
//		before_get_in_island = 1;
//		currItem = forward;
//		distance_set = 5000;
//		P67 = 1;
//	}

//	else if ((before_get_in_island == 1) && (distance_set == 0) && (angle_set == 0))
//	{

//		before_get_in_island = 0;
//		currItem = line;
//		//P67 = 0;
//	}

//	// 圆环部分
////	else if ( (adc_guiyi[0] > 50) && (get_in_island_FLAG == 0)
////		&& (adc_guiyi[2]+ adc_guiyi[3] > 198) && (adc_guiyi[1]+ adc_guiyi[4] < 70)
////		&& (distance_set == 0) )
////
////	{
////		get_in_island_FLAG = 1;
////		currItem = Lisland;
////		distance_set = 3000;
////		P67 = 1;
////	}
////	else if ( (adc_guiyi[5] > 48) && (get_in_island_FLAG == 0)
////		&& (adc_guiyi[2]+ adc_guiyi[3] > 198) && (adc_guiyi[1]+ adc_guiyi[4] < 90)
////		&& (distance_set == 0) )
////	{
////		get_in_island_FLAG = 10;
////		currItem = Risland;
////		distance_set = 3000;
////		P67 = 1;
////	}
////
////
////	else if ( (adc_guiyi[0] > 80) && (get_in_island_FLAG == 1) && (distance_set == 0) )
////	{
////		currItem = L_at_island;
////		distance_set = 2000;//50000走到出小圆环一点
////		get_in_island_FLAG = 2;//3
////		P67 = 0;
////	}
////
////	else if ( (adc_guiyi[5] > 70) && (get_in_island_FLAG == 10) && (distance_set == 0) )
////	{
////		currItem = R_at_island;
////		distance_set = 2000;
////		get_in_island_FLAG = 20;//30
////		P67 = 0;
////	}
////
////
////
////	else if ( (get_in_island_FLAG == 2) && (distance_set == 0) )
////	{
////		currItem = line;
////		distance_set = 50000;//50000走到出小圆环一点
////		get_in_island_FLAG = 3;//3
////		//P67 = 0;
////	}
////
////	else if ( (get_in_island_FLAG == 20) && (distance_set == 0) )
////	{
////		currItem = line;
////		distance_set = 50000;
////		get_in_island_FLAG = 30;//30
////		//P67 = 0;
////	}
////
////
////	else if ( (get_in_island_FLAG == 3) && (distance_set == 0) )
////	{
////		//P67 = 0;
////		get_in_island_FLAG = 0;
////	}
////
////	else if ( (get_in_island_FLAG == 30) && (distance_set == 0) )
////	{
////		//P67 = 0;
////		get_in_island_FLAG = 0;
////	}



//	// 冲十字
//	else if ( ( (adc_guiyi[1] + adc_guiyi[4]) > 150) && (get_in_island_FLAG == 0)
//	          && (before_get_in_island == 0) && (distance_set == 0) && (angle_set == 0))
//	{
//		currItem = cross;
//		distance_set = 3000;
//		//P67 = 1;
//	}


//	/*


//	if ( (get_in_island_FLAG == 3) &&  (distance_set == 0) )
//	{
//		if ( adc_guiyi[0] + adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] > 430)
//		{
//			currItem = L_out_island;//走直线
//			distance_set = 8000;
//			get_in_island_FLAG = 4;
//		}

//	}
//	if ( (get_in_island_FLAG == 30) &&  (distance_set == 0) )
//	{
//		if ( adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] + adc_guiyi[5] > 430)
//		{
//			currItem = R_out_island;//走直线
//			distance_set = 8000;
//			get_in_island_FLAG = 4;
//		}

//	}


//	if (get_in_island_FLAG == 4)
//	{
//		if (distance_set == 0)
//		{
//			currItem = line;
//			distance_set = 5000;
//			get_in_island_FLAG = 0;


//		}
//	}
//	*/






////	if (get_in_island_FLAG == 4)
////	{
////		if (distance_set == 0)
////		{
////			currItem = line;
////			get_in_island_FLAG = 0;
////		}

////	}



////	else if((adc_guiyi[1] - adc_guiyi[4] > 50) && (adc_guiyi[2] + adc_guiyi[3] < 130))
////	{
////		currItem = left90;

////	}

////	else if((adc_guiyi[4] - adc_guiyi[1] > 50) && (adc_guiyi[2] + adc_guiyi[3] < 130))
////	{
////		currItem = right90;

////	}



//}
