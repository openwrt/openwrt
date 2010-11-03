

int mps_message_send(struct mps_fifo *fifo, struct mps_message *msg)
{
}

int mps_message_recv(struct mps_fifo *fifo, struct mps_message *msg)
{
	mps_fifo_out(fifo, &msg->header, sizeof(msg->header));
	mps_fifo_out(fifo, &msg->data, mps_message_size(msg));
}

struct mps_message *msg mps_message_recv_alloc(struct mps_fifo *fifo)
{
	struct mps_message *msg;

	mps_fifo_out(fifo, &msg->header, sizeof(msg->header));
	mps_fifo_out(fifo, &msg->data, mps_message_size(msg));
}
