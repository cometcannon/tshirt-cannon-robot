package manager;

import robot.Networker;
import robot.KeepAliveThread;

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

        robotState.setNetworker(new Networker(robotState));

        CommandObserver observer = new CommandObserver(robotState, uiState.getCurrentCommand());
        uiState.getCurrentCommand().addObserver(observer);

        Networker networker = new Networker(robotState);
        KeepAliveThread keepAliveThread = new KeepAliveThread(robotState);
//        keepAliveThread.start();
    }
}
