package manager;

import robot.Networker;
import robot.KeepAliveThread;

public class RobotManager implements Runnable
{
    ApplicationState applicationState;
    RobotState robotState;

    protected RobotManager(ApplicationState applicationState,
                           RobotState robotState)
    {
        this.applicationState = applicationState;
        this.robotState = robotState;
    }

    @Override
    public void run()
    {
        Networker networker = new Networker(robotState);
        Thread networkerThread = new Thread(networker);
        networkerThread.start();
        
        KeepAliveThread keepAliveThread = new KeepAliveThread(robotState);
        Thread keepAlive = new Thread(keepAliveThread);
        keepAlive.start();
    }
}
