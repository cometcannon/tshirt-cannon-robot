package manager;

import robot.Networker;

public class RobotManager
{
    ApplicationState applicationState;
    RobotState robotState;
    UIState uiState;

    protected RobotManager(ApplicationState applicationState,
                           RobotState robotState,
                           UIState uiState)
    {
        this.applicationState = applicationState;
        this.robotState = robotState;
        this.uiState = uiState;

        Networker networker = new Networker(robotState);
    }
}
