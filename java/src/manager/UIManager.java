package manager;

import gui.MainFrame;

public class UIManager
{
    ApplicationState applicationState;
    RobotState robotState;
    UIState uiState;

    protected UIManager(ApplicationState applicationState,
                        RobotState robotState,
                        UIState uiState)
    {
        this.applicationState = applicationState;
        this.robotState = robotState;
        this.uiState = uiState;

        MainFrame frame = new MainFrame(uiState);
    }
}
