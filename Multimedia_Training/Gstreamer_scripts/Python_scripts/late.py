import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib

def on_bus_message(bus, message, loop):
    t = message.type
    if t == Gst.MessageType.EOS:
        print("End of stream")
        loop.quit()
    elif t == Gst.MessageType.ERROR:
        err, debug = message.parse_error()
        print(f"Error: {err}, Debug: {debug}")
        loop.quit()
    elif t == Gst.MessageType.WARNING:
        err, debug = message.parse_warning()
        print(f"Warning: {err}, Debug: {debug}")
    elif t == Gst.MessageType.STATE_CHANGED:
        old_state, new_state, pending_state = message.parse_state_changed()
        print(f"State changed: {old_state} -> {new_state}")

def main():
    # Initialize GStreamer
    Gst.init(None)

    # Create a pipeline
    pipeline_str = "videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12 ! autovideosink"
    pipeline = Gst.parse_launch(pipeline_str)

    # Set up a main loop
    loop = GLib.MainLoop.new(None, False)

    # Set up bus to handle messages
    bus = pipeline.get_bus()
    bus.add_signal_watch()
    bus.connect("message", on_bus_message, loop)

    # Start the pipeline
    pipeline.set_state(Gst.State.PLAYING)

    try:
        # Run the main loop
        loop.run()
    except KeyboardInterrupt:
        # Handle keyboard interrupt
        pass
    finally:
        # Stop the pipeline
        pipeline.set_state(Gst.State.NULL)
        loop.quit()

if __name__ == "__main__":
    main()

