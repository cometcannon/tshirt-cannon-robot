package edu.utdallas.cometcannon.robot;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import edu.utdallas.cometcannon.manager.RobotState;

import edu.utdallas.cometcannon.robot.command.FireCannonCommand;
import edu.utdallas.cometcannon.robot.command.VelocityVectorCommand;

class Mapping
{
    ////////////////////////////////////////
    //   Buttons
    // 0) A
    // 1) B
    // 2) X
    // 3) Y
    // 4) left bumper
    // 5) right bumper
    // 6) start
    // 7) back
    // 8) left stick (press)
    // 9) right stick (press)

    private int[] buttons = new int[10];
    private Controller controller;
    private boolean foundMapping = false;

    public Mapping(Controller controller)
    {
        this.controller = controller;
        createMapping();
    }

    private void createMapping()
    {
        String osName = System.getProperty("os.name");
        foundMapping = true;

        if (osName.equalsIgnoreCase("Mac OS X")) {
            buttons = new int[] {11, 12, 13, 14, 8, 9, 4, 5, 7, 6};
        }

        else if (osName.equalsIgnoreCase("Linux")) { //TODO: Need to populate button array
            buttons = new int[] {};
        }

        else if (osName.equalsIgnoreCase("windows"))
        {}

        else
            foundMapping = false;
    }

    public boolean isMappingFound()
    {
        return foundMapping;
    }

    public boolean isButtonAPressed()
    {
        return controller.isButtonPressed(buttons[0]);
    }

    public boolean isButtonBPressed()
    {
        return controller.isButtonPressed(buttons[1]);
    }

    public boolean isButtonXPressed()
    {
        return controller.isButtonPressed(buttons[2]);
    }

    public boolean isButtonYPressed()
    {
        return controller.isButtonPressed(buttons[3]);
    }

}

public class RemoteController implements Runnable
{
    boolean keepAlive = true;

    long lastTriggerCommandTime = System.currentTimeMillis();

    public static Controller controller;
    RobotState robotState;
    Mapping buttonMap;

    public RemoteController(RobotState robotState)
    {
        this.robotState = robotState;

        try {
            Controllers.create();
        } catch (LWJGLException ex) {
            ex.printStackTrace();
        }

        if (Controllers.getControllerCount() > 0) {
            controller = Controllers.getController(0);
            System.out.println("Found controller");
            buttonMap = new Mapping(controller);
        }
        else
            System.out.println("Could not find any controllers");

    }

    @Override
    public void run()
    {
        calibrate();

        while (keepAlive) {

            if(controller.poll())
            {
                //checkButtons();
                handleButtonPress();
                handleTriggerPress();
                handleAxisMovement();
            }
            else
            {
                System.out.println("Controller Disconnected: Stopping RemoteController Thread");
                keepAlive = false;
            }

            try { Thread.sleep(100); } catch (InterruptedException ex) { }
        }
    }

    private void checkButtons()
    {
        for (int i = 0; i < controller.getButtonCount(); i++)
            if (controller.isButtonPressed(i))
                System.out.println("Button " + i + " is pressed");
    }

    private void handleButtonPress()
    {
        if (buttonMap.isButtonAPressed()) {
            System.out.println("A pressed");
        }

        if (buttonMap.isButtonBPressed()) {
            System.out.println("B pressed");
        }

        if (buttonMap.isButtonXPressed()) {
            System.out.println("X pressed");
        }

        if (buttonMap.isButtonYPressed()) {
            System.out.println("Y pressed");
        }
    }

    private void handleTriggerPress()
    {
        if (System.currentTimeMillis() - lastTriggerCommandTime < 1000)
            return;

        lastTriggerCommandTime = System.currentTimeMillis();

        int leftTrigger = (int)controller.getZAxisValue();
        int rightTrigger = (int)controller.getRZAxisValue();

        if(leftTrigger < 0.9 || rightTrigger < 0.9)
            return;

        System.out.println(leftTrigger + " " + rightTrigger ); //TODO: Start using logger

        robotState.addNewCommand(new FireCannonCommand());
    }

    private void handleAxisMovement()
    {
        int v_x = (int) (-1 * controller.getYAxisValue() * 127);
        int v_y = (int) (-1 * controller.getXAxisValue() * 127);
        int w_z = (int) (-1 * controller.getRXAxisValue() * 127);

        System.out.printf("%d, %d, %d\n", v_x, v_y, w_z);

        robotState.addNewCommand(new VelocityVectorCommand(v_x, v_y, w_z));
    }

    private void calibrate()
    {
        for (int i = 1; i < controller.getAxisCount(); i++) {
            controller.setDeadZone(i, 0.2f);
        }
    }
}
