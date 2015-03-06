package robot;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import manager.RobotState;

import robot.command.KillRobotCommand;
import robot.command.FireCannonCommand;
import robot.command.VelocityVectorCommand;

public class RemoteController implements Runnable
{
    //////////////////////////////////////// 
    //   Axes
    // 1) x (left stick) (inverted)
    // 2) y (left stick)
    // 4) x (right stick)
    // 5) y (right stick) (inverted)
    private final int axes[] = {1, 2, 4, 5};
    
    //////////////////////////////////////// 
    //   Axes
    // 3) left trigger
    // 6) right trigger
    private final int triggers[] = {3, 6};
    
    //////////////////////////////////////// 
    //   Buttons
    // 0) A
    // 1) B
    // 2) X
    // 3) Y
    // 4) left bumper
    // 5) right bumper
    // 6) select
    // 7) mode
    // 8) left stick (press)
    // 9) right stick (press)
    
    long lastButtonCommandTime = 0;
    long lastTriggerCommandTime = 0;
    long lastVelocityCommandTime = 0;

    public static Controller controller;
    RobotState robotState;
    
    public RemoteController(RobotState robotState)
    {
        this.robotState = robotState;
        
        try {
            Controllers.create();
        } catch (LWJGLException ex) {
            ex.printStackTrace();
            System.exit(0); // XXX we probably need to handle this more gracefully
        }
        
        Controllers.poll();
        
        if (Controllers.getControllerCount() > 0) {
            controller = Controllers.getController(0);
            System.out.println("Found controller");
        } else {
            System.out.println("Could not find any controllers");
        }
    }
    
    @Override
    public void run()
    {
        calibrate();

        while (true) {
            controller.poll();
            
            for (int i = 0; i < controller.getButtonCount(); i++)
                if (controller.isButtonPressed(i))
                    handleButtonPress(i);
            
            for (int i = 0; i < triggers.length; i++)
                if (controller.getAxisValue(triggers[i]) > 0.9)
                    handleTriggerPress();
            
            boolean kill = true;
            for (int i = 0; i < axes.length; i++) {
                double axisValue = controller.getAxisValue(axes[i]);

                if (axisValue > 0.1 || axisValue < -0.1) {
                    handleAxisMovement();
                    kill = false;
                }
            }
            
            if (kill) 
                robotState.addNewCommand(new KillRobotCommand());

            try { Thread.sleep(100); } catch (InterruptedException ex) { }
        }
    }
    
    private void handleButtonPress(int button)
    {
    }
    
    private void handleTriggerPress()
    {
        if (System.currentTimeMillis() - lastTriggerCommandTime < 1000)
            return;
        
        lastTriggerCommandTime = System.currentTimeMillis();

        robotState.addNewCommand(new FireCannonCommand());
    }
    
    private void handleAxisMovement()
    {
        if (System.currentTimeMillis() - lastVelocityCommandTime < 50)
            return;
        
        lastVelocityCommandTime = System.currentTimeMillis();
        
        int v_x = (int) (-1 * controller.getAxisValue(2) * 127);
        int v_y = (int) (controller.getAxisValue(1) * 127);
        int w_z = (int) (controller.getAxisValue(4) * 127);
        
        robotState.addNewCommand(new VelocityVectorCommand(v_x, v_y, w_z));
    }
    
    private void calibrate()
    {
        for (int i = 1; i < controller.getAxisCount(); i++) {
            controller.setDeadZone(i, 0.2f);
        }
    }
}
