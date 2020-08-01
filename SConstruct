#!python

import os
env = Environment(ENV = os.environ)
 
env['AR'] = 'arm-none-eabi-ar'
env['AS'] = 'arm-none-eabi-as'
env['CC'] = 'arm-none-eabi-gcc'
env['CXX'] = 'arm-none-eabi-g++'
env['LINK'] = 'arm-none-eabi-gcc'
env['RANLIB'] = 'arm-none-eabi-ranlib'
env['OBJCOPY'] = 'arm-none-eabi-objcopy'
env['PROGSUFFIX'] = '.elf'

stm32cubef1_hal_path = '../stm32cubef1/Drivers/STM32F1xx_HAL_Driver/'
stm32cubef1_cmsis_path = '../stm32cubef1/Drivers/CMSIS/'
stm32cubef1_middlewears_path = '../stm32cubef1/Middlewares/ST/'


stm_family='STM32F103XB'
#stm_device='STM32F103X6'
stm_device='STM32F103XB'

# include locations
env.Append(CPPPATH = [
    '#inc',
    '#src/parser', 
    '#' + stm32cubef1_hal_path + 'Inc',
    '#' + stm32cubef1_cmsis_path +'Include',
    '#' + stm32cubef1_cmsis_path +'Device/ST/STM32F1xx/Include',
    '#' + stm32cubef1_middlewears_path +'STM32_USB_Device_Library/Core/Inc',
    '#' + stm32cubef1_middlewears_path +'STM32_USB_Device_Library/Class/CDC/Inc', 
    ])

env.Append(LIBPATH = [
    'lib'
    ])

# compiler flags
env.Append(CCFLAGS = [
    '-mcpu=cortex-m3',
    '-march=armv7-m',
    '-mthumb',
    '-Os',
    '-std=gnu11',
    '-Wall',
    '-g',
    '-DDISABLE_ECHO'
])

Export('env')

SConscript(['src/parser/SConscript'])


# linker flags
#    '-Wl,--gc-sections,-Map=main.elf.map,-cref,-u,Reset_Handler',
env.Append(LINKFLAGS = [
    #'-mcpu=cortex-m3',
    '-mthumb',
    '-specs=nano.specs',
    '-specs=nosys.specs',
    '-Wl,--gc-sections,-Map=main.elf.map,-cref,-u,Reset_Handler,--trace',
     '-T', 'src/gcc/linker/'+ stm_device + '_FLASH.ld'
    ]) 

# defines
env.Append(CPPDEFINES = [
    stm_family.replace('X', 'x'),
])


env.VariantDir('build/stm32/drv/', stm32cubef1_hal_path+'Src', duplicate=0)
env.VariantDir('build/stm32/usb/', stm32cubef1_middlewears_path+ 'STM32_USB_Device_Library/Core/Src', duplicate=0)
env.VariantDir('build/stm32/usb/cdc', stm32cubef1_middlewears_path +'STM32_USB_Device_Library/Class/CDC/Src', duplicate=0)

env.Library('#lib/libstm32',
                   [
                       'build/stm32/drv/stm32f1xx_hal.c',
                       'build/stm32/drv/stm32f1xx_hal_gpio.c',
                       'build/stm32/drv/stm32f1xx_hal_rcc.c',
                       'build/stm32/drv/stm32f1xx_hal_cortex.c',
                       'build/stm32/drv/stm32f1xx_hal_dma.c',
                       'build/stm32/drv/stm32f1xx_hal_flash.c',
                       'build/stm32/drv/stm32f1xx_hal_gpio.c',
                       'build/stm32/drv/stm32f1xx_hal_pwr.c',
                       'build/stm32/drv/stm32f1xx_hal_uart.c',
                       'build/stm32/drv/stm32f1xx_hal_pcd.c',
                       'build/stm32/drv/stm32f1xx_hal_pcd_ex.c',
                       'build/stm32/drv/stm32f1xx_hal_tim.c',
                       'build/stm32/drv/stm32f1xx_hal_tim_ex.c',
                       'build/stm32/drv/stm32f1xx_hal_rcc.c',
                       'build/stm32/drv/stm32f1xx_hal_rcc_ex.c',
                       'build/stm32/drv/stm32f1xx_hal_i2c.c',
                       
                       'build/stm32/drv/stm32f1xx_ll_usb.c',
                       
                       'build/stm32/usb/usbd_core.c',
                       'build/stm32/usb/usbd_ctlreq.c',
                       'build/stm32/usb/usbd_ioreq.c',
                       
                       'build/stm32/usb/cdc/usbd_cdc.c',

                   ])

env.VariantDir('build/src/', 'src', duplicate=0)


#print(env.Dump())
# build everything
prg = env.Program(
    target = 'main',
    LIBS=['libstm32'],
    source = [
        'build/src/main.c',
        'build/src/board.c',
        'build/src/time.c',
        'build/src/i2c.c',
        'build/src/max44009.c',
        'build/src/terminal.c',
        'build/src/shuttest.c',
        'build/src/shuttest_terminal.c',
        'build/src/stm32f1xx_it.c',
        'build/src/system_stm32f1xx.c',
        'build/src/usbd_cdc_interface.c',
        'build/src/usbd_conf.c',
        'build/src/usbd_desc.c',
        'build/src/gcc/startup_' + stm_device.lower() + '.s'
    ]
)
 
# binary file builder
def arm_generator(source, target, env, for_signature):
    return '$OBJCOPY -O binary %s %s'%(source[0], target[0])

env.Append(BUILDERS = {
    'Objcopy': Builder(
        generator=arm_generator,
        suffix='.bin',
        src_suffix='.elf'
    )
})
 
env.Objcopy(prg)

