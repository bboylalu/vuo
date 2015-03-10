/**
 * @file
 * VuoUrl interface.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifdef __cplusplus
extern "C"
{
#endif

char *VuoUrl_normalize(const char *url);
bool VuoUrl_get(const char *url, void **data, unsigned int *dataLength);

#ifdef __cplusplus
}
#endif
