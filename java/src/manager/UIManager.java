package manager;

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
    }
}
