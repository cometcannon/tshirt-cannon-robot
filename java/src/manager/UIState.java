package manager;

public class UIState
{
    ObservableCommand currentCommand;
    
    protected UIState()
    {
        currentCommand = new ObservableCommand("", 0);
    }

    public void addCommand(String commandType)
    {
        addCommand(commandType, 0);
    }

    public void addCommand(String commandType, int commandMagnitude)
    {
        currentCommand.setValue(commandType, commandMagnitude);
    }

    public ObservableCommand getCurrentCommand()
    {
        return currentCommand;
    }
}
