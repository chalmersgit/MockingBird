#ifndef MOCKINGBIRD_SRC_BANANACLIENT_H
#define MOCKINGBIRD_SRC_BANANACLIENT_H

#define BC_RECEIPT "./src/bananaclient/"

class BananaClient {

	public:
		BananaClient();
		~BananaClient();

		void loop();

	protected:

	private:
		int bindable();

		int sockfd;

};

#endif
