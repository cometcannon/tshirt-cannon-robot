package edu.utdallas.cometcannon.robot;

import java.util.concurrent.*;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import edu.utdallas.cometcannon.robot.command.*;
import edu.utdallas.cometcannon.gui.*;

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

        else if (osName.equalsIgnoreCase("Linux")) {
            buttons = new int[] {0, 1, 2, 3, 4, 5, -1, 8, 10, 11};
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
    Mapping buttonMap;
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
            controller = Controllers.getController(7);
            System.out.println("Found controller");
            buttonMap = new Mapping(controller);
        }
        else
            System.out.println("Could not find any controllers");

    }

    public void Terminate()
    {
        keepAlive = false;
    }

    @Override
    public void run()
    {
        calibrate();

        while (keepAlive) {

            if(controller.poll())
            {
                //checkButtons(); //Use this to check your system's button layout
                handleButtonPress();
                handleTriggerPress();
                handleAxisMovement();
            }
            else
            {
                System.out.println("Controller Disconnected: Stopping RemoteController Thread");
                robotCommandQueue.offer(new VelocityVectorCommand(0, 0, 0));
                Terminate();
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
            robotCommandQueue.offer(new IncreasePressureCommand());
        }

        if (buttonMap.isButtonBPressed()) {
            //robotCommandQueue.offer(new DebugAVRCommand());
        }

        if (buttonMap.isButtonXPressed()) {
            robotCommandQueue.offer(new DebugAtherosCommand());
        }

        if (buttonMap.isButtonYPressed()) {
            robotCommandQueue.offer(new SendPressureCommand());
        }
    }

    private void handleTriggerPress()
    {
        if (System.currentTimeMillis() - lastTriggerCommandTime < 800)
            return;

        lastTriggerCommandTime = System.currentTimeMillis();

        int leftTrigger = (int)controller.getZAxisValue();
        int rightTrigger = (int)controller.getRZAxisValue();

        if(leftTrigger < 0.9 || rightTrigger < 0.9)
            return;

        System.out.println(leftTrigger + " " + rightTrigger ); //TODO: Start using logger
        robotCommandQueue.offer(new FireCannonCommand());
    }

    private void handleAxisMovement()
    {
        int v_x = (int) (-1 * controller.getYAxisValue() * 127);
        int v_y = (int) (-1 * controller.getXAxisValue() * 127);
        int w_z = (int) (-1 * controller.getRXAxisValue() * 127);

        robotCommandQueue.offer(new VelocityVectorCommand(v_x, v_y, w_z));

        MainFrame.V_X = v_x;
        MainFrame.V_Y = v_y;
    }

    private void calibrate()
    {
        for (int i = 1; i < controller.getAxisCount(); i++)
            controller.setDeadZone(i, 0.2f);
    }
}
