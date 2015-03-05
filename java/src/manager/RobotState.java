package manager;

import robot.Networker;

public class RobotState
{
    Networker networker;
    
    protected RobotState()
    {

    }

    public Networker getNetworker()
    {
        return networker;
    }

    public void setNetworker(Networker networker)
    {
        this.networker = networker;
    }
}
