/** @defgroup rtcmacro rtcmacro
 * @ingroup rtc
 * @brief Constants for programming the i8254 Timer. Needs to be completed.
 * @{
 *
 */
#define BIT(n)             (0x01<<(n))
#define IRQ_RTC             8       /**< @brief RTC IRQ line */

#define RTC_ADDR_REG        0x70    /**< @brief Address register */
#define RTC_DATA_REG        0x71    /**< @brief Data register */

#define REGA                0xA     /**< @brief RTC's A register */
#define REGA_UIP            BIT(7)  /**< @brief REGA's UIP flag - when 1, don't access date/hour registers */

#define SECREG              0x00    /**< @brief Seconds register */
#define MINREG              0x02    /**< @brief Minutes register */
#define HOUREG              0x04    /**< @brief Hours register */
#define DAYREG              0x07    /**< @brief Days register */
#define MONREG              0x08    /**< @brief Month register */
#define YEAREG              0x09    /**< @brief Year register */

/**@}*/
