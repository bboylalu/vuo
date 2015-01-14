/**
 * @file
 * vuo-debug implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#include <getopt.h>
#include <Vuo/Vuo.h>


class TelemetryLogger : public VuoRunnerDelegate
{
	void receivedTelemetryStats(unsigned long utime, unsigned long stime)
	{
		printf("stats: %lu %lu\n", utime, stime);
	}
	void receivedTelemetryNodeExecutionStarted(string nodeIdentifier)
	{
		printf("nodeExecutionStarted: %s\n", nodeIdentifier.c_str());
	}
	void receivedTelemetryNodeExecutionFinished(string nodeIdentifier)
	{
		printf("nodeExecutionFinished: %s\n", nodeIdentifier.c_str());
	}
	void receivedTelemetryInputPortUpdated(string portIdentifier, bool receivedEvent, bool receivedData, string dataSummary)
	{
		printf("inputPortUpdated: %s %d %d %s\n", portIdentifier.c_str(), receivedEvent, receivedData, dataSummary.c_str());
	}
	void receivedTelemetryOutputPortUpdated(string portIdentifier, bool sentData, string dataSummary)
	{
		printf("outputPortUpdated: %s %d %s\n", portIdentifier.c_str(), sentData, dataSummary.c_str());
	}
	void receivedTelemetryPublishedOutputPortUpdated(VuoRunner::Port *port, bool sentData, string dataSummary)
	{
		printf("publishedOutputPortUpdated: %s %d %s\n", port->getName().c_str(), sentData, dataSummary.c_str());
	}
	void receivedTelemetryError(string message)
	{
		printf("error: %s\n", message.c_str());
	}
	void lostContactWithComposition(void)
	{
		printf("lostContactWithComposition\n");
	}
};


int main (int argc, char * const argv[])
{
	bool doPrintHelp = false;
	string executablePath;

	static struct option options[] = {
		{"help", no_argument, NULL, 0},
		{NULL, no_argument, NULL, 0}
	};
	int optionIndex=-1;
	while((getopt_long(argc, argv, "", options, &optionIndex)) != -1)
	{
		switch(optionIndex)
		{
			case 0:  // --help
				doPrintHelp = true;
				break;
		}
	}

	if (doPrintHelp)
	{
		printf("Tool for debugging compositions. Runs the given composition executable file and logs telemetry data to the console.\n\n"
			   "Usage: %s [options] file\n\n"
			   "Options:\n"
			   "  --help                       Display this information.\n",
			   argv[0]);
	}
	else
	{
		if (optind == argc)
		{
			fprintf(stderr, "%s: no input file\n", argv[0]);
			return 1;
		}
		executablePath = argv[optind];

		VuoRunner * runner = VuoRunner::newSeparateProcessRunnerFromExecutable(executablePath);

		TelemetryLogger *logger = new TelemetryLogger();
		runner->setDelegate(logger);

		runner->start();
		runner->waitUntilStopped();
	}

	return 0;
}