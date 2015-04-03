package edu.utdallas.cometcannon.manager;

import edu.utdallas.cometcannon.robot.Networker;
import edu.utdallas.cometcannon.robot.RemoteController;

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

        RemoteController controller = new RemoteController(robotState);
        Thread controllerThread = new Thread(controller);
        controllerThread.start();
    }
}
