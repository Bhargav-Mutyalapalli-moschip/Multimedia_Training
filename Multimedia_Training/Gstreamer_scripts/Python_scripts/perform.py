import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject

Gst.init(None)

pipeline_description = "videotestsrc ! autovideosink"
pipeline = Gst.parse_launch(pipeline_description)

# Create a bus to get events from the GStreamer pipeline
bus = pipeline.get_bus()

def on_message(bus, message):
    if message.type == Gst.MessageType.EOS:
        print("End of stream")
        loop.quit()
    elif message.type == Gst.MessageType.ERROR:
        err, debug_info = message.parse_error()
        print(f"Error: {err}, Debug information: {debug_info}")
        loop.quit()
    elif message.type == Gst.MessageType.STATE_CHANGED:
        if message.src == pipeline:
            old_state, new_state, pending_state = message.parse_state_changed()
            print(f"Pipeline state changed from {Gst.Element.state_get_name(old_state)} to {Gst.Element.state_get_name(new_state)}")

# Connect the message handler function to the bus
bus.connect("message", on_message)

# Set the pipeline to the playing state
pipeline.set_state(Gst.State.PLAYING)

# Run the GStreamer main loop
loop = GObject.MainLoop()
try:
    loop.run()
except KeyboardInterrupt:
    # Handle keyboard interrupt to gracefully stop the pipeline
    pass
finally:
    # Set the pipeline to the NULL state before exiting
    pipeline.set_state(Gst.State.NULL)
    print("Pipeline stopped")

# Calculate and print the average frames per second
duration = pipeline.query_duration(Gst.Format.TIME)[1]
num_frames = pipeline.query_position(Gst.Format.FRAMES)[1]
fps = num_frames / (duration / Gst.SECOND)
print(f"Average Frames Per Second: {fps}")

