from distutils.core import setup, Extension

setup(
    name='aliendown',
    version='0.1.0',
    description='When does the narwhal bacon? At sundown.',
    author='Vicent Marti',
    author_email='vicent@github.com',
    license='MIT',
    ext_modules=[Extension('aliendown', [
        'src/aliendown.c',
        'src/array.c',
        'src/buffer.c',
        'src/markdown.c',
        'src/html.c',
#        'src/html_smartypants.c',
        'src/autolink.c'
    ])]
)
