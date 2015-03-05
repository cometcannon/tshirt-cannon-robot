package manager;

import gui.MainFrame;

public class UIManager implements Runnable
{
    ApplicationState applicationState;
    RobotState robotState;

    protected UIManager(ApplicationState applicationState,
                        RobotState robotState)
    {
        this.applicationState = applicationState;
        this.robotState = robotState;
    }
    
    @Override
    public void run()
    {
        MainFrame frame = new MainFrame(robotState);
    }
}
