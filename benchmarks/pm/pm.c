#include <annot.h>

#define SIG_BUFF_SIZE 50				// Size of the signal ring buffer
#define BUFFER_OFFSET 1					// Overlap count for ring buffer
#define SAMPLING_INTERVAL_MS 10 // Sampling interval in seconds
#define LEARN_TIME_MS 81920			// Learning time in milliseconds
// Learning time chosen so sample_count = 8192 (2^13), allowing average computation via bit shift instead of division
#define GRI_THRESHOLD_MS 12000 // Default GRI value in milliseconds
#define LRI_THRESHOLD_MS 20500 // Default LRI value in milliseconds

// For division replacement using bit-shift
#define Q 24
#define SCALE (1 << Q)						// 16777216
#define RECIP_6000 (SCALE / 6000) // 2796

typedef enum
{
	LEARNING = 0,	 // Learning state
	DETECTING = 1, // Detecting state
	IGNORING = 2,	 // Ignoring state
	PACING = 3		 // Pacing state
} PmState;

// Global Variables
int sig_buff[SIG_BUFF_SIZE];
int snapshot_buff[SIG_BUFF_SIZE];
PmState state = LEARNING; // Initial state is Learning
int sig_idx = 1;
int time_counter = 0;
int lri_counter = 0;
int gri_counter = 0;

int activation_flag = 0;
int lowest_slope_sum = 0;
int lowest_slope_count = 0;
int detection_threshold;

void detect_activation(int lowest_slope)
{
	if (lowest_slope < detection_threshold)
	{
		activation_flag = 1;
		// printf("Activation detected! Lowest slope: %d / Threshold: %d\n", lowest_slope, detection_threshold);
	}
	else
	{
		activation_flag = 0;
	}
}

int main(int new_sample)
{
	// 2. Process : Activation detection and pacing decision

	// 2.1 : Process Signal

	// Get lowest slope from snapshot

	int lowest_slope = (sig_buff[1] - sig_buff[0]);
	for (int i = 2; i < SIG_BUFF_SIZE; i++)
	{
		ANNOT_MAXITER(SIG_BUFF_SIZE);
		int slope = (sig_buff[i] - sig_buff[i - 1]);
		if (slope < lowest_slope)
		{
			lowest_slope = slope;
		}
	}

	// 2.2 : State Machine for Pacing Decision
	SWITCH(state)
	{
	case LEARNING:
		if (time_counter < LEARN_TIME_MS)
		{
			// Self Looping in LEARNING state

			// accumulate lowest slope values
			lowest_slope_sum += (int)lowest_slope;
			lowest_slope_count++;
		}
		else
		{
			// Transition 1 : LEARNING -> DETECTING

			// calculate and set detection threshold
			// detection_threshold = (int)(lowest_slope_sum / lowest_slope_count) * 4.5; // %%%%%% why 4.5?
			// detection_threshold = (int)(lowest_slope_sum / lowest_slope_count);
			// detection_threshold = (lowest_slope_sum * RECIP_6000) >> Q;
			detection_threshold = lowest_slope_sum >> 13; // divide by 8192
			// allowing average computation via bit shift instead of division

			// reset LRI counter and set state to DETECTING
			lri_counter = 0;
			state = DETECTING;
		}
		break;

	case DETECTING:
		// detect activation
		detect_activation(lowest_slope);

		if (lri_counter <= LRI_THRESHOLD_MS)
		{
			if (activation_flag == 0)
			{
				// Transition 1 : DETECTING -> DETECTING (self-loop)
			}
			else
			{
				// Transition 2 : DETECTING -> IGNORING
				// reset GRI and LRI and set state to IGNORING
				lri_counter = 0;
				gri_counter = 0;
				state = IGNORING;
			}
		}
		else
		{
			// Transition 3 : DETECTING -> PACING
			state = PACING;
		}

		break;

	case IGNORING:
		if (gri_counter <= GRI_THRESHOLD_MS)
		{
			// Self Looping in IGNORING state
		}
		else
		{
			// Transition 1 : IGNORING -> DETECTING
			state = DETECTING;
		}
		break;

	case PACING:
		// detect activation
		detect_activation(lowest_slope);

		if (activation_flag == 0)
		{
			// Transition 1 : PACING -> PACING (self-loop)

			// What happens when there is no activation detected? Currently nothing, will stay in this state forever. Need to add a timeout?
		}
		else
		{
			// Transition 2 : PACING -> IGNORING
			// reset GRI and LRI and set state to IGNORING
			lri_counter = 0;
			gri_counter = 0;
			state = IGNORING;
		}
		break;

	default:
		return 1;
		break;
	}

	// 2.3 : Update counters
	time_counter += SAMPLING_INTERVAL_MS;
	lri_counter += SAMPLING_INTERVAL_MS;
	gri_counter += SAMPLING_INTERVAL_MS;

	// 3. Actuate : Send stimulation signal based on pacing decision

	if (state == PACING)
	{
		return 0;
	}
}