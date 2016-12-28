#include "rtcmacro.h"

/** @addtogroup rtc rtc
 * @{
 *
 *
 */

/**

@brief Reads current time or date (according to user specification).

Defined in rtc_get.S
@param time_or_date integer: 0 to read time, else to read date
@return eax pointer to first element of array containing wanted data ([day,month,year] or [hours,minutes,seconds])

 */
eax _rtc_get (time_or_date)
{
	return eax;
}
