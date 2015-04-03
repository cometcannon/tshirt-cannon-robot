package edu.utdallas.cometcannon.robot;

import java.util.concurrent.*;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import edu.utdallas.cometcannon.robot.command.*;

class Mapping
{
    int[] axes = new int[4];
    int[] triggers = new int[2];
    int[] buttons = new int[10];

    private void createMapping()
    {

        String osName = System.getProperty("os.name");

        if (osName.equalsIgnoreCase("Mac OS X")) {
            axes = new int[] {1, 0, 2, 5};
            triggers = new int[] {4, 5};
            buttons = new int[] {};
        }

        else if (osName.equalsIgnoreCase("Linux")) { //TODO: Need to populate these arrays
            axes = new int[] {};
            triggers = new int[] {};
            buttons = new int[] {};
        }

    }

    public int[] getAxes()
    {
        return axes;
    }

    public int[] getTriggers()
    {
        return triggers;
    }

    public int[] getButtons()
    {
        return buttons;
    }
}

public class RemoteController implements Runnable
{
    ////////////////////////////////////////
    //   Axes
    // 1) x (left stick) (inverted)
    // 0) y (left stick) (inverted)
    // 2) x (right stick) (inverted)
    // 3) y (right stick)
    private final int axes[] = {1, 0, 2, 5};

    ////////////////////////////////////////
    //   Axes
    // 4) left trigger
    // 5) right trigger
    private final int triggers[] = {4, 5};

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
    private ArrayBlockingQueue<Command> robotCommandQueue;

    public RemoteController(ArrayBlockingQueue<Command> robotCommandQueue)
    {
        this.robotCommandQueue = robotCommandQueue;

        try {
            Controllers.create();
        } catch (LWJGLException ex) {
            ex.printStackTrace();
        }

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
            if(controller.poll()) {
                for (int i = 0; i < controller.getButtonCount(); i++)
                    if (controller.isButtonPressed(i))
                        handleButtonPress(i);

                for (int i = 0; i < triggers.length; i++)
                    if (controller.getAxisValue(triggers[i]) > 0.9)
                        handleTriggerPress();

                handleAxisMovement();
            } else {
                System.out.println("Controller Disconnected: Stopping the program");
                System.exit(0);
            }

            try { Thread.sleep(100); } catch (InterruptedException ex) { }
        }
    }

    private void handleButtonPress(int button)
    {
        //System.out.printf("%d\n", controller.isButtonPressed(button));
    }

    private void handleTriggerPress()
    {
        if (System.currentTimeMillis() - lastTriggerCommandTime < 1000)
            return;

        lastTriggerCommandTime = System.currentTimeMillis();

        robotCommandQueue.offer(new FireCannonCommand());
    }

    private void handleAxisMovement()
    {
        if (System.currentTimeMillis() - lastVelocityCommandTime < 50)
            return;

        lastVelocityCommandTime = System.currentTimeMillis();

        int v_x = (int) (-1 * controller.getYAxisValue() * 127);
        int v_y = (int) (-1 * controller.getXAxisValue() * 127);
        int w_z = (int) (-1 * controller.getRXAxisValue() * 127);

        robotCommandQueue.offer(new VelocityVectorCommand(v_x, v_y, w_z));
    }

    private void calibrate()
    {
        for (int i = 1; i < controller.getAxisCount(); i++)
            controller.setDeadZone(i, 0.2f);
    }
}
