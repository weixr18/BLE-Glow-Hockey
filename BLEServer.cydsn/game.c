/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "game.h"


// Game task
void GlowHockeyTask(){
    
    uint score_0 = 0;
    uint score_1 = 0;
   
    uint16 ballPositionX = (uint16) (tableWidth * BALL_INITIAL_RATE_X);
    uint16 ballPositionY = (uint16) (tableHeight * BALL_INITIAL_RATE_Y);
    int16 ballSpeedX = 0;
    int16 ballSpeedY = 0;
    
    uint16 player0X = (int) (tableWidth * PLAYER_0_INITIAL_RATE_X);
    uint16 player0Y = (int) (tableHeight * PLAYER_0_INITIAL_RATE_Y);
    uint16 player0XLast = player0X;
    uint16 player0YLast = player0Y;
    int16 player0SpeedX = 0;
    int16 player0SpeedY = 0;
    
    uint16 player1X = (int) (tableWidth * PLAYER_1_INITIAL_RATE_X);
    uint16 player1Y = (int) (tableWidth * PLAYER_1_INITIAL_RATE_Y);
    uint16 player1XLast = player1X;
    uint16 player1YLast = player1Y;
    int16 player1SpeedX = 0;
    int16 player1SpeedY = 0;
    
    uint32 receivedPosition;
    uint32 sendBallPosition;
    uint32 sendOppositePosition;
    uint64 bytesToSend;

    

    for(;;){
        if(gameState == GAME_START && isReceivePosition){

            /********** receive position *************/
            xQueueReceive(bleQueueHandle, &receivedPosition, portTICK_PERIOD_MS);
            isReceivePosition = false;
            
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                player0X = ((uint16) receivedPosition) & USER_POSITION_MASK;
                player0Y = ((uint16)(receivedPosition >> 12)) & USER_POSITION_MASK;
                
                player0SpeedX = player0X - player0XLast;
                player0SpeedY = player0Y - player0YLast;
                player0XLast = player0X;
                player0YLast = player0Y;
            }
            else{
                // player 1
                player1X = tableWidth - (((uint16) receivedPosition) & USER_POSITION_MASK);
                player1Y = tableHeight - ((uint16)(receivedPosition >> 12) & USER_POSITION_MASK);
                
                player1SpeedX = player1X - player1XLast;
                player1SpeedY = player1Y - player1YLast;
                player1XLast = player1X;
                player1YLast = player1Y;
            }
            
            /********** game logic *************/
            
             // 小球左右边框碰撞事件
            if (ballPositionX - gamePaddingLeft <= ballSize) {
                ballSpeedX = abs(ballSpeedX);
            } else if (ballPositionX + gamePaddingRight >= tableWidth - ballSize) {
                ballSpeedX = - abs(ballSpeedX);
            }

            // 小球上下边框碰撞事件
            if (ballPositionY - gamePaddingTop <= ballSize) {
                // 0方得分
                if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                    
                    // send scores
                    score_0 += 1;
                    printf("GAME: Player 0 scored!! %d -- %d\r\n", score_0, score_1);
                   
                    uint64 scores = ((score_1 & ((uint64)0x07)) << 3) | (score_0 & ((uint64)0x07));
                    uint64 operand = GH_CC_N_SCORE;
                    bytesToSend = ((scores << 5) | operand) & (uint64)0x000007ff;
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend,
                        bleConnectionHandle0
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(1000);
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend,
                        bleConnectionHandle1
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(1000);
                    
                    // return to start position
                    
                    //if (scoreA >= 7) {
                    //    isOver = true;
                    //} 
                    //else {
                    //isStart = false;
                
                        ballPositionX = (int16) (tableWidth * BALL_INITIAL_RATE_X);
                        ballPositionY = (int16) (tableHeight * BALL_INITIAL_RATE_Y);
                        ballSpeedX = 0;
                        ballSpeedY = 0;
                        player0X = (int16) (tableWidth * PLAYER_0_INITIAL_RATE_X);
                        player0Y = (int16) (tableHeight * PLAYER_0_INITIAL_RATE_Y);
                        player1X = (int16) (tableWidth * PLAYER_1_INITIAL_RATE_X);
                        player1Y = (int16) (tableHeight * PLAYER_1_INITIAL_RATE_Y);
                        
                    //}
                }
                // 正常碰撞
                else{
                    ballSpeedY = abs(ballSpeedY);
                }
            } 
            else if (ballPositionY + gamePaddingBottom >= tableHeight - ballSize) {
                printf("GAME: Reach bottom.\r\n");
                // 1方得分
                if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                    score_1 += 1;
                    printf("GAME: Player 1 scored!! %d -- %d\r\n", score_0, score_1);
                    
                    uint64 scores = ((score_1 & ((uint64)0x07)) << 3) | (score_0 & ((uint64)0x07));
                    uint64 operand = GH_CC_N_SCORE;
                    bytesToSend = ((scores << 5) | operand) & (uint64)0x000007ff;
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend,
                        bleConnectionHandle0
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(1000);
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend,
                        bleConnectionHandle1
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(1000);
                    
                    //if (scoreB >= 7) {
                    //    isOver = true;
                    //} else {
                        //isStart = false;
                                        
                        ballPositionX = (int16) (tableWidth * BALL_INITIAL_RATE_X);
                        ballPositionY = (int16) (tableHeight * BALL_INITIAL_RATE_Y);
                        ballSpeedX = 0;
                        ballSpeedY = 0;
                        player0X = (int16) (tableWidth * PLAYER_0_INITIAL_RATE_X);
                        player0Y = (int16) (tableHeight * PLAYER_0_INITIAL_RATE_Y);
                        player1X = (int16) (tableWidth * PLAYER_1_INITIAL_RATE_X);
                        player1Y = (int16) (tableHeight * PLAYER_1_INITIAL_RATE_Y);
                        
                    //}
                }
                // 正常碰撞
                else{
                    ballSpeedY = - abs(ballSpeedY);
                }
                
            }


            // 小球和玩家碰撞事件
            double ball_player0_dis = sqrt(pow(ballPositionX - player0X, 2)
                                         + pow(ballPositionY - player0Y, 2) );
            
            double ball_player1_dis = sqrt(pow(ballPositionX - player1X, 2)
                                         + pow(ballPositionY - player1Y, 2) );
            
            // with player 0
            if (ball_player0_dis < ballSize + playerCircleSize) {

                printf("GAME: Bounce player 0 !!\r\n");
                //if (!isStart) {
                //    isStart = true;
                //}
                printf("GAME: player 0 speed: (%d, %d)\r\n", player0SpeedX, player0SpeedY);
                printf("GAME: speed before:(%d, %d)\r\n", ballSpeedX, ballSpeedX);

                ballSpeedX += (int16) (2 * PLAYER_CIRCLE_MASS * (player0SpeedX - ballSpeedX)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedX += (int16) (ballPositionX - player0X) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_player0_dis) / (ballSize + playerCircleSize);

                ballSpeedY += (int16) (2 * PLAYER_CIRCLE_MASS * (player0SpeedY - ballSpeedY)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedY += (int16) (ballPositionY - player0Y) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_player0_dis) / (ballSize + playerCircleSize);
                
                printf("GAME: speed after:(%d, %d)\r\n", ballSpeedX, ballSpeedY);
            }
            
            // with player 1
            if (ball_player1_dis < ballSize + playerCircleSize) {

                printf("GAME: Bounce player 1 !!\r\n");
                //if (!isStart) {
                //    isStart = true;
                //}
                printf("GAME: player 1 speed: (%d, %d)\r\n", player1SpeedX, player1SpeedY);
                printf("GAME: speed before:(%d, %d)\r\n", ballSpeedX, ballSpeedX);

                ballSpeedX += (int16) (2 * PLAYER_CIRCLE_MASS * (player1SpeedX - ballSpeedX)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedX += (int16) (ballPositionX - player1X) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_player1_dis) / (ballSize + playerCircleSize);

                ballSpeedY += (int16) (2 * PLAYER_CIRCLE_MASS * (player1SpeedY - ballSpeedY)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedY += (int16) (ballPositionY - player1Y) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_player1_dis) / (ballSize + playerCircleSize);
                
                printf("GAME: speed after:(%d, %d)\r\n", ballSpeedX, ballSpeedY);
            }
            

            // 防止speed过大
            if (ballSpeedX > ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedX = (int16) (ballSize * BALL_SPEED_MAX_RATE);
            } else if (ballSpeedX < -ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedX = -(int16) (ballSize * BALL_SPEED_MAX_RATE);
            }

            if (ballSpeedY > ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedY = (int16) (ballSize * BALL_SPEED_MAX_RATE);
            } else if (ballSpeedY < -ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedY = -(int16) (ballSize * BALL_SPEED_MAX_RATE);
            }

            //防止speed过小
            //if (isStart) {
                if (ballSpeedX >= 0 && ballSpeedX < ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedX = (int16) (ballSize * BALL_SPEED_MIN_RATE);
                } else if (ballSpeedX < 0 && ballSpeedX > -ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedX = -(int16) (ballSize * BALL_SPEED_MIN_RATE);
                }
                if (ballSpeedY >= 0 && ballSpeedY < ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedY = (int16) (ballSize * BALL_SPEED_MIN_RATE);
                } else if (ballSpeedY < 0 && ballSpeedY > -ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedY = -(int16) (ballSize * BALL_SPEED_MIN_RATE);
                }
            //}

            ballPositionY += ballSpeedY;
            ballPositionX += ballSpeedX;
            
    
            // send user position
            
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                sendOppositePosition = 0;
                sendOppositePosition |= (uint32)(player1X & USER_POSITION_MASK);
                sendOppositePosition |= ((uint32)(player1Y & USER_POSITION_MASK) << 12);
                
                sendBallPosition = 0;
                sendBallPosition |= (uint32)(ballPositionX & USER_POSITION_MASK);
                sendBallPosition |= ((uint32)(ballPositionY & USER_POSITION_MASK) << 12);
                
                bytesToSend = 0;
                bytesToSend |= sendOppositePosition;
                bytesToSend |= ((uint64)sendBallPosition) << 32;

                if((isUser_0_PosStartNotification & NOTIFY_BIT_MASK)){
                    SendBleNotification(
                        CY_BLE_GH_POSITION_PLAYER_POSITION_N_CHAR_HANDLE, 
                        &bytesToSend,
                        bleConnectionHandle0
                    );
                }  
                else{
                    printf("GAME: Not send user 0 position.\r\n");
                }
                Cy_BLE_ProcessEvents();
            }
            else{
                // player 1
                sendOppositePosition = 0;
                sendOppositePosition |= ((uint32)tableWidth - (uint32)(player0X & USER_POSITION_MASK));
                sendOppositePosition |= ((uint32)tableHeight - (uint32)(player0Y & USER_POSITION_MASK)) << 12;
                
                sendBallPosition = 0;                                
                sendBallPosition |= ((uint32)tableWidth - (uint32)(ballPositionX & USER_POSITION_MASK));
                sendBallPosition |= ((uint32)tableHeight - (uint32)(ballPositionY & USER_POSITION_MASK)) << 12;                
                bytesToSend = 0;
                bytesToSend |= sendOppositePosition;
                bytesToSend |= ((uint64)sendBallPosition) << 32;

                // if the notification is allowed , send the notification
                if((isUser_1_PosStartNotification & NOTIFY_BIT_MASK)){
                    SendBleNotification(
                        CY_BLE_GH_POSITION_PLAYER_POSITION_N_CHAR_HANDLE, 
                        &bytesToSend,
                        bleConnectionHandle1
                    );
                }  
                else{
                    printf("GAME: Not send user 1 position.\r\n");
                }
                Cy_BLE_ProcessEvents();
                

            }
        }
        
        
        else{
            taskYIELD();
        }
    }
}



/* [] END OF FILE */
